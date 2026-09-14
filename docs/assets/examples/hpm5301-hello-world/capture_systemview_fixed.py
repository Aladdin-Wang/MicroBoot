from pathlib import Path
import json
from mklink.device import connect
p=Path('E:/software/HPM5300/sdk_env_v1.11.0/hpm_sdk/samples/hello_world')
with connect(port='COM488',mcu='hpm5301',axf=str(p/'hpm5301evklite_flash_xip_rtos/output/demo.elf'),project_root=str(p)) as dev:
    started=dev.systemview_start(addr=0x82a88,channel=1,search_size=1024)
    try:
        report=dev.systemview_read(duration=5)
    finally:
        dev.systemview_stop()
(p/'.mklink/work/20260912-hpm-doc/evidence/systemview-events-fixed.json').write_text(json.dumps(report,indent=2,default=str),encoding='utf-8')
print(json.dumps({k:v for k,v in report.items() if k!='events'},default=str))
