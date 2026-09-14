from pathlib import Path
import json
from mklink.device import connect
from mklink.dump_benchmark import measure
root=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
output=root/'.mklink/work/20260912-hpm-doc/evidence/benchmark.json'
results=[]
for speed in ('low','medium','high'):
    for name,regions in [('float1',[(0x804f8,4)]),('float2',[(0x804f8,8)]),('float16',[(0x804f8,64)]),('ram4096',[(0x80544,4096)])]:
        with connect(port='COM488',mcu='hpm5301',project_root=str(root)) as dev:
            row=measure(dev,regions,duration=5,period=.000001,speed_profile=speed)
            row.update(case=name,speed=speed)
        results.append(row)
        output.write_text(json.dumps(results,indent=2),encoding='utf-8')
        print(json.dumps({'case':name,'speed':speed,'sample_hz':row['sample_hz'],'p99':row['p99_interval_us']}),flush=True)
with connect(port='COM488',mcu='hpm5301',project_root=str(root)) as dev:
    dev.set_debug_speed('medium')
