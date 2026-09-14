from pathlib import Path
import json,struct,hashlib
from elftools.elf.elffile import ELFFile
from mklink.device import connect
p=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
build=p/'hpm5301evklite_flash_xip_trap/output'
with (build/'demo.elf').open('rb') as f:
    symbols={s.name:s['st_value'] for s in ELFFile(f).get_section_by_name('.symtab').iter_symbols() if s.name in ['superwatch_tick','superwatch_build_id','superwatch_wave','_SEGGER_RTT','trap_unlock','trap_request','trap_record','demo_illegal_instruction']}
report={'symbols':{k:hex(v) for k,v in symbols.items()},'bin_size':(build/'demo.bin').stat().st_size,'sha256':hashlib.sha256((build/'demo.bin').read_bytes()).hexdigest()}
with connect(port='COM488',mcu='hpm5301',project_root=str(p)) as dev:
    report['flash']=dev.flash(str(build/'demo.bin'),target_part='HPM5301xEGx',base_address=0x80000400,board='hpm5301evklite',verify=True)
    report['build_id']=hex(struct.unpack('<I',dev.read_memory(symbols['superwatch_build_id'],4))[0])
    assert report['build_id']=='0x20260915'
    assert dev.read_memory(symbols['_SEGGER_RTT'],16).startswith(b'SEGGER RTT')
    report['rtt_signature_pass']=True
(p/'.mklink/work/20260912-hpm-doc/evidence/flash-trap-verified.json').write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report,indent=2))
