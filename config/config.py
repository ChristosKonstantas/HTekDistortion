import json
import sys
from pathlib import Path

OLD_NAME = "PluginTemplate"
CONFIG_FILE = "config.json"

def kill_process(msg: str):
    print(f"ERROR: {msg}")
    sys.exit(1)

def b(v: bool) -> str:
        return "TRUE" if v else "FALSE"

if __name__ == "__main__":
    
    script_dir = Path(__file__).parent.resolve()   # config/
    root = script_dir.parent.resolve()             # project root
    config_path = script_dir / CONFIG_FILE         # config/config.json

    if not config_path.exists():
        kill_process(f"{CONFIG_FILE} not found in {script_dir}")

    # Read config.json
    try:
        cfg = json.loads(config_path.read_text(encoding="utf-8"))
    except json.JSONDecodeError as e:
        kill_process(f"Invalid JSON: {e}")

    required = ["plugin_name", "company_name", "plugin_code", "manufacturer_code"]
    for k in required:
        if k not in cfg:
            kill_process(f"Missing required key '{k}' in {config_path}")

    new_name = cfg["plugin_name"]

    print("This script is intended to be executed ONCE on a fresh clone !")
    print(f"Plugin name changed:")
    print(f"  {OLD_NAME} -> {new_name}")
    print()

    """
        (1) Rename directories (deepest first)
    """
    dirs = sorted([p for p in root.rglob("*") if p.is_dir() and OLD_NAME in p.name],
                   key=lambda p: len(p.parts),
                   reverse=True)

    for d in dirs:
        new_dir = d.with_name(d.name.replace(OLD_NAME, new_name))
        print(f"renamed directory:  {d} -> {new_dir}")
        d.rename(new_dir)

    """
        (2) Rename files
    """
    files = [p for p in root.rglob("*") if p.is_file() and OLD_NAME in p.name]

    for f in files:
        new_file = f.with_name(f.name.replace(OLD_NAME, new_name))
        print(f"renamed file: {f} -> {new_file}")
        f.rename(new_file)

    """
        (3) Replace text inside files
    """
    exts = {".cpp", ".h", ".cmake", ".md"}
    special_names = {"CMakeLists.txt"}

    replacements = { OLD_NAME: new_name,
                    "COMPANY_NAME kiztam": f"COMPANY_NAME {cfg['company_name']}",
                    "PLUGIN_CODE M1lt": f"PLUGIN_CODE {cfg['plugin_code']}",
                    "PLUGIN_MANUFACTURER_CODE WLSD": f"PLUGIN_MANUFACTURER_CODE {cfg['manufacturer_code']}",
                    "IS_SYNTH FALSE": f"IS_SYNTH {b(cfg.get('is_synth', False))}",
                    "NEEDS_MIDI_INPUT FALSE": f"NEEDS_MIDI_INPUT {b(cfg.get('needs_midi_input', False))}",
                    "NEEDS_MIDI_OUTPUT FALSE": f"NEEDS_MIDI_OUTPUT {b(cfg.get('needs_midi_output', False))}",
                    "FORMATS VST3": f"FORMATS {cfg.get('formats', 'VST3')}",
                   }

    for f in root.rglob("*"):
        if not f.is_file():
            continue

        if f.name not in special_names and f.suffix not in exts:
            continue

        if ".git" in f.parts or "build" in f.parts:
            continue

        try:
            text = f.read_text(encoding="utf-8")
        except UnicodeDecodeError:
            continue

        new_text = text
        for old, new in replacements.items():
            new_text = new_text.replace(old, new)

        if new_text != text:
            f.write_text(new_text, encoding="utf-8")
            print(f"text: {f}")

    print("\n Config completed successfully.")