import re
from pathlib import Path

Import("env")

# Only matches IRAM_ATTR from function declarations, not definitions.
RE_IRAM_ATTR = re.compile(r" IRAM_ATTR (?=[^(]+\([^)]*\);)")


def remove_duplicate_iram_attrs(path):
    contents = path.read_text()
    result = RE_IRAM_ATTR.sub(" ", contents)
    if result != contents:
        path.write_text(result)
        print(f"Patched {path}")


# Remove the unused include directive from LilyGoEPD47/src/libjpeg/libjpeg.c.
# The active code in that file never uses SPIFFS; the only SPIFFS-dependent
# function is inside #if 0.
def remove_unused_include(path):
    path.write_text(path.read_text().replace('#include "esp_spiffs.h"', ""))
    print(f"Patched {path}")


lib_deps = Path(
    env.get("PROJECT_LIBDEPS_DIR", Path(env["PROJECT_DIR"]) / ".piolibdeps")
)
src_dir = lib_deps / env["PIOENV"] / "LilyGoEPD47" / "src"

if src_dir.is_dir():
    for header in src_dir.glob("*.h"):
        remove_duplicate_iram_attrs(header)
    for source in src_dir.glob("*.c"):
        remove_duplicate_iram_attrs(source)
    remove_unused_include(src_dir / "libjpeg" / "libjpeg.c")
