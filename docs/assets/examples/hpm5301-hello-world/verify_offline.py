from pathlib import Path
import json,hashlib,struct,time
from mklink.device import connect
p=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
expected=(p/'hpm5301evklite_flash_xip_rtos/output/demo.bin').read_bytes()
with connect(port='COM488',mcu='hpm5301',project_root=str(p)) as dev:
    actual=b''.join(dev.read_memory(0x80000400+i,min(4096,len(expected)-i)) for i in range(0,len(expected),4096))
    assert actual==expected
    build=struct.unpack('<I',dev.read_memory(0x82a84,4))[0]
    tick1=struct.unpack('<I',dev.read_memory(0x82a80,4))[0]
    time.sleep(.1)
    tick2=struct.unpack('<I',dev.read_memory(0x82a80,4))[0]
    assert build==0x20260914 and tick2>tick1
r={'backend':'offline HPM ROM API','bytes_verified':len(actual),'sha256':hashlib.sha256(actual).hexdigest(),'flash_readback_pass':True,'build':hex(build),'ticks':[tick1,tick2],'running_pass':True}
(p/'.mklink/work/20260912-hpm-doc/evidence/offline-verified.json').write_text(json.dumps(r,indent=2),encoding='utf-8')
print(json.dumps(r,indent=2))
