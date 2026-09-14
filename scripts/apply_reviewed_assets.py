"""One-time refinement after visually inspecting the 28 unclear source assets."""
import json
from pathlib import Path
from organize_docs import safe_path, ROOT, REPORT

if __name__ == '__main__':
    refinements = json.loads((REPORT / 'reviewed-assets.json').read_text(encoding='utf-8'))
    mapping = json.loads((REPORT / 'path-map.json').read_text(encoding='utf-8'))
    hashes = json.loads((REPORT / 'binary-hashes.json').read_text(encoding='utf-8'))
    pending = json.loads((REPORT / 'pending-review.json').read_text(encoding='utf-8'))
    corrections = {'docs/images/pending-review/' + name:'docs/' + new for name,new in refinements.items()}
    for old, new in corrections.items():
        assert safe_path(old).is_file() and not safe_path(new).exists(), (old,new)
    for old, new in corrections.items():
        safe_path(new).parent.mkdir(parents=True, exist_ok=True)
        safe_path(old).rename(safe_path(new))
        hashes[new] = hashes.pop(old)
    mapping = {old:corrections.get(new,new) for old,new in mapping.items()}
    pending = [item for item in pending if item['new'] not in corrections]
    for name, data in [('path-map.json',mapping),('binary-hashes.json',hashes),('pending-review.json',pending)]:
        (REPORT / name).write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
    print(f'{len(corrections)} assets identified visually; {len(pending)} remain pending')
