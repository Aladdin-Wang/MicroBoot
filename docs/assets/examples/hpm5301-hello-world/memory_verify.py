from pathlib import Path
import json,struct,time
from mklink.device import connect
root=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
out=root/'.mklink/work/20260912-hpm-doc/evidence/memory-verified.json'
expected=struct.pack('<1024I',*(0x48504D00^i for i in range(1024)))
with connect(port='COM488',mcu='hpm5301',project_root=str(root)) as dev:
    before=dev.read_memory(0x80544,4096)
    assert before==expected,'Initial 4KB pattern mismatch'
    patch=bytes.fromhex('12345678A5A55A5A')
    try:
        dev.write_memory(0x80544,patch)
        after=dev.read_memory(0x80544,4096)
        assert after==patch+expected[8:],'Write or neighboring content mismatch'
    finally:
        dev.write_memory(0x80544,expected[:8])
    restored=dev.read_memory(0x80544,4096)
    assert restored==expected,'Restore mismatch'
    report={'bytes_checked':4096,'initial_pattern_pass':True,'write_address':'0x00080544','write_hex':patch.hex(),'write_readback_pass':True,'unchanged_neighbors_pass':True,'restored_full_region_pass':True}
out.write_text(json.dumps(report,indent=2),encoding='utf-8')
print(json.dumps(report))
