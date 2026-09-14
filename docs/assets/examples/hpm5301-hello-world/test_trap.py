from pathlib import Path
import json, struct, time
from mklink.device import connect
p=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
e=p/'.mklink/work/20260912-hpm-doc/evidence'
s={k:int(v,16) for k,v in json.loads((e/'flash-trap-verified.json').read_text())['symbols'].items()}
with connect(port='COM488',mcu='hpm5301',project_root=str(p)) as dev:
    assert struct.unpack('<I',dev.read_memory(s['superwatch_build_id'],4))[0]==0x20260915
    before=dev.read_memory(s['trap_record'],20)
    assert before==bytes(20)
    dev.write_memory(s['trap_unlock'],struct.pack('<I',0x48504D53))
    dev.write_memory(s['trap_request'],struct.pack('<I',0x54524150))
    time.sleep(1.2)
    record=struct.unpack('<5I',dev.read_memory(s['trap_record'],20))
    report=dict(zip(['mcause','handler_return_pc','mtval','mstatus','marker'],map(hex,record)))
    report['fault_pc']=hex(record[1]-4)
    report['instruction_hex']=dev.read_memory(record[1]-4,4).hex()
    assert report['instruction_hex']=='ffffffff'
    assert record[0]==2 and record[4]==0x54524150
    report['illegal_instruction_pass']=True
(e/'trap-verified.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report,indent=2))
