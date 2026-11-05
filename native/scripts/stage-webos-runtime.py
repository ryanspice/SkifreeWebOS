#!/usr/bin/env python3
"""Copy only explicit SDL-webOS and PNG runtimes/licenses into a new native stage."""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import subprocess


def sha(path):
    return hashlib.sha256(path.read_bytes()).hexdigest()


def check_arm(path):
    header = subprocess.check_output(['readelf', '-h', str(path)], text=True, timeout=20)
    if not re.search(r'Class:\s+ELF32', header) or not re.search(r'Machine:\s+ARM\s*$', header, re.M):
        raise RuntimeError('Refusing non-ARM32 file: ' + str(path))


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--stage', type=Path, required=True)
    parser.add_argument('--sdl-prefix', type=Path, required=True)
    parser.add_argument('--png-prefix', type=Path, required=True)
    parser.add_argument('--png-license', type=Path, required=True)
    args = parser.parse_args()
    stage = args.stage.resolve(strict=True)
    check_arm(stage / 'skifree-native')
    if not (stage / 'appinfo.json').is_file():
        raise RuntimeError('Native stage appinfo is missing')
    mappings = [
        (args.sdl_prefix / 'lib/libSDL2-2.0.so.0', 'lib/libSDL2-2.0.so.0'),
        (args.png_prefix / 'lib/libpng16.so.16', 'lib/libpng16.so.16'),
        (args.sdl_prefix / 'share/licenses/SDL2/LICENSE.txt', 'licenses/SDL2-LICENSE.txt'),
        (args.png_license, 'licenses/libpng-LICENSE.txt'),
    ]
    for source, dest in mappings:
        source = source.resolve(strict=True)
        if dest.startswith('lib/'):
            check_arm(source)
        target = stage / dest
        if target.exists() and sha(target) != sha(source):
            raise RuntimeError('Existing staged file differs; choose a new stage: ' + str(target))
    receipt = []
    for source, dest in mappings:
        source = source.resolve(strict=True)
        target = stage / dest
        target.parent.mkdir(parents=True, exist_ok=True)
        if not target.exists():
            shutil.copy2(source, target)
        receipt.append({'source': str(source), 'staged_path': dest, 'sha256': sha(target),
                        'bytes': target.stat().st_size})
    report = {'files': receipt, 'platform_libraries_copied': False,
              'device_tested': False, 'codec_reason': 'SDL_webOSLoadLibraries calls IMG_InitPNG, which dlopens libpng16.so.16'}
    (stage / 'RUNTIME-FILES.json').write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
