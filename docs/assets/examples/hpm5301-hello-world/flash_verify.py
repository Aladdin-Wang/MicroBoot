from pathlib import Path
import hashlib,json,time,struct
from mklink.device import connect
root=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
evidence=root/'.mklink/work/20260912-hpm-doc/evidence'
binfile=root/'hpm5301evklite_flash_xip_debug/output/demo.bin'
report={'firmware':str(binfile),'size':binfile.stat().st_size,'sha256':hashlib.sha256(binfile.read_bytes()).hexdigest()}
with connect(port='COM488',mcu='hpm5301',project_root=str(root)) as dev:
    start=time.monotonic()
    report['flash']=dev.flash(str(binfile),target_part='HPM5301xEGx',base_address=0x80000400,board='hpm5301evklite',verify=True)
    report['flash_wall_s']=time.monotonic()-start
    report['status_before']=struct.unpack('<III',dev.read_memory(0x80538,12))
    time.sleep(1)
    report['status_after']=struct.unpack('<III',dev.read_memory(0x80538,12))
    report['wave_snapshot']=struct.unpack('<16f',dev.read_memory(0x804f8,64))
    assert report['status_after'][0]>report['status_before'][0]
    assert report['status_after'][1]==0x20260912
    assert all(abs(x)<=100.01 for x in report['wave_snapshot'])
(evidence/'flash-verified.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report,indent=2))
