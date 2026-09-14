from pathlib import Path
import csv,json,math,struct,time
from mklink.device import connect
from mklink.dump_memory import DumpMemoryStreamSession
root=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
out=root/'.mklink/work/20260912-hpm-doc/evidence'
rows=[]
with connect(port='COM488',mcu='hpm5301',project_root=str(root)) as dev:
    dev.set_debug_speed('medium')
    # A separate snapshot checks known benchmark data before starting the stream.
    block=dev.read_memory(0x80544,64)
    assert struct.unpack('<16I',block)==tuple(0x48504D00 ^ i for i in range(16))
    stream=DumpMemoryStreamSession(dev._bridge,[(0x804f8,64)],.001)
    stream.start()
    start=time.monotonic()
    try:
        while time.monotonic()-start<5:
            frames=stream.read_frames(max_bytes=262144)
            for f in frames:
                assert not f['flags']
                values=struct.unpack('<16f',f['regions'][0][1])
                rows.append([f['timestamp_us'],*values])
            if not frames: time.sleep(.001)
    finally:
        stream.stop()
    stats=stream.stats
assert len(rows)>1000
assert all(math.isfinite(x) and abs(x)<=100.01 for r in rows for x in r[1:])
crossings=[r[0] for prev,r in zip(rows,rows[1:]) if prev[1]<0<=r[1]]
periods=[(b-a)/1e6 for a,b in zip(crossings,crossings[1:])]
assert len(periods)>=2 and all(.98<p<1.02 for p in periods)
assert all(min(r[i+1] for r in rows)<-99 and max(r[i+1] for r in rows)>99 for i in range(16))
report={'samples':len(rows),'periods_s':periods,'channel_min':[min(r[i+1] for r in rows) for i in range(16)],'channel_max':[max(r[i+1] for r in rows) for i in range(16)],'integrity':stats}
with (out/'wave16-capture.csv').open('w',newline='',encoding='utf-8') as f:
    w=csv.writer(f);w.writerow(['timestamp_us']+[f'ch{i:02}' for i in range(16)]);w.writerows(rows)
(out/'wave16-validation.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report,indent=2))
