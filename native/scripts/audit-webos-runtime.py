#!/usr/bin/env python3
"""Bounded, read-only ELF dependency audit; target sysroot evidence is not a TV test."""
import argparse
import hashlib
import json
import os
from pathlib import Path
import re
import subprocess


def tool(*args):
    return subprocess.check_output(args, text=True, stderr=subprocess.STDOUT, timeout=20)


def target_path(path, sysroot):
    """Resolve a target absolute symlink inside the sysroot, never on the host."""
    path = Path(os.path.normpath(path))
    for _ in range(16):
        try:
            path.relative_to(sysroot)
        except ValueError:
            return None
        if not path.is_symlink():
            return path if path.is_file() else None
        link = Path(os.readlink(path))
        path = sysroot / str(link).lstrip('/') if link.is_absolute() else path.parent / link
        path = Path(os.path.normpath(path))
    return None


def find_library(name, sysroot):
    if '/' in name:
        return None
    for folder in ('usr/lib', 'lib'):
        found = target_path(sysroot / folder / name, sysroot)
        if found:
            return found
    return None


def inspect(path):
    header = tool('readelf', '-h', str(path))
    dynamic = tool('readelf', '-d', str(path))
    program = tool('readelf', '-l', str(path))
    version_info = tool('readelf', '--version-info', str(path))
    needs_section = version_info.split('Version needs section', 1)
    needed_versions = sorted(set(re.findall(r'Name: (\S+)', needs_section[1]))) if len(needs_section) > 1 else []
    match = re.search(r'Requesting program interpreter: ([^\]]+)', program)
    return {
        'path': str(path),
        'sha256': hashlib.sha256(path.read_bytes()).hexdigest(),
        'elf32_arm': bool(re.search(r'Class:\s+ELF32', header) and re.search(r'Machine:\s+ARM\s*$', header, re.M)),
        'flags': re.search(r'Flags:\s*(.+)', header).group(1),
        'interpreter': match.group(1) if match else None,
        'needed': re.findall(r'\(NEEDED\).*?\[([^\]]+)\]', dynamic),
        'needed_symbol_versions': needed_versions,
    }


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--binary', type=Path, required=True)
    parser.add_argument('--sdl', type=Path, required=True)
    parser.add_argument('--sysroot', type=Path, required=True)
    parser.add_argument('--extra', type=Path, action='append', default=[], help='Explicitly bundled dlopen library')
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    sysroot = args.sysroot.resolve(strict=True)
    extra = [p.resolve(strict=True) for p in args.extra]
    bundled = {p.name: p for p in [args.sdl.resolve(strict=True)] + extra}
    def resolve_library(name):
        return bundled.get(name) or find_library(name, sysroot)
    pending = [args.binary.resolve(strict=True), args.sdl.resolve(strict=True)] + extra
    seen = set()
    records = []
    missing = []
    while pending:
        path = pending.pop(0)
        if path in seen:
            continue
        if len(seen) >= 64:
            raise RuntimeError('Dependency closure exceeds audit limit')
        seen.add(path)
        item = inspect(path)
        records.append(item)
        if item['interpreter']:
            interpreter = target_path(sysroot / item['interpreter'].lstrip('/'), sysroot)
            if not interpreter:
                missing.append({'from': str(path), 'name': item['interpreter'], 'kind': 'interpreter'})
            elif interpreter not in seen:
                pending.append(interpreter)
        for name in item['needed']:
            library = resolve_library(name)
            if not library:
                missing.append({'from': str(path), 'name': name, 'kind': 'DT_NEEDED'})
            else:
                pending.append(library)
    strings = tool('strings', str(args.sdl))
    candidates = sorted(set(re.findall(r'^lib[\w+.-]+\.so(?:\.[\w.-]+)?$', strings, re.M)))
    backends = [{'name': name, 'resolved_path': str(p) if (p := resolve_library(name)) else None,
                 'bundled': name in bundled}
                for name in candidates if name != 'libSDL2-2.0.so.0']
    report = {
        'kind': 'static SDK runtime audit',
        'sysroot': str(sysroot),
        'dt_needed_and_interpreter_resolved': not missing,
        'all_elf32_arm': all(item['elf32_arm'] for item in records),
        'records': records,
        'missing': missing,
        'sdl_dynamic_library_string_candidates': backends,
        'limits': [
            'SDL and explicitly supplied preloaded codecs are dlopen audit roots.',
            'Dynamic library strings identify candidates, not proof that every backend needs all of them.',
            'Matching SDK paths/architecture do not establish compatibility with an installed TV runtime.',
            'TV launch, remote mappings, GL drivers, selected backend, lifecycle and performance remain untested.'
        ],
        'device_tested': False,
    }
    output = json.dumps(report, indent=2) + '\n'
    if args.out:
        args.out.parent.mkdir(parents=True, exist_ok=True)
        args.out.write_text(output, encoding='utf-8')
        print(json.dumps({'report': str(args.out), 'records': len(records), 'missing': missing,
                          'all_elf32_arm': report['all_elf32_arm']}))
    else:
        print(output, end='')
    return 0 if not missing and report['all_elf32_arm'] else 1


if __name__ == '__main__':
    raise SystemExit(main())
