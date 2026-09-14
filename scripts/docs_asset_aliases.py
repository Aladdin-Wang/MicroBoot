"""MkDocs hook: old public image paths resolve to unchanged source assets."""
import json
from pathlib import Path
import shutil


def on_post_build(config, **kwargs):
    root = Path(__file__).resolve().parents[1]
    output = Path(config['site_dir']).resolve()
    mapping = json.loads((root / 'maintenance/docs-organization/path-map.json').read_text(encoding='utf-8'))
    for old, new in mapping.items():
        if not old.startswith('docs/images/') or not new.startswith('docs/images/'):
            continue
        source = output / new.removeprefix('docs/')
        destination = output / old.removeprefix('docs/')
        if not source.resolve().is_relative_to(output) or not destination.resolve().is_relative_to(output):
            raise ValueError('Image alias path escapes build directory')
        destination.parent.mkdir(parents=True, exist_ok=True)
        shutil.copy2(source, destination)
