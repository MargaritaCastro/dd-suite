"""Pre-run check: ddo reads its own copy of the instances (ddo_simplificado/resources/),
so if it drifts from DataInstances/ the C++ vs ddo comparison silently compares
different instances.

Usage:  python3 DataInstances/check_rust_resources.py     (from the repo root)

Knapsack is stored in two formats (DD-suite: n / capacity / weights / values; ddo:
"n capacity" + n lines "value weight"), so it is compared field by field. Sequencing
uses the same format on both sides and is compared byte for byte.
"""

import os
import sys

REPO = os.path.dirname(os.path.abspath(os.path.join(__file__, "..")))
DI = os.path.join(REPO, "DataInstances")
RS = os.path.join(REPO, "ddo_simplificado", "resources")


def _no_blank(path):
    with open(path) as f:
        return [line.strip() for line in f if line.strip()]


def check_knapsack():
    """DD-suite and ddo must encode the same n, capacity, weights and values."""
    di_dir = os.path.join(DI, "Knapsack", "Standard")
    rs_dir = os.path.join(RS, "knapsack")
    issues = []

    for name in sorted(os.listdir(di_dir)):
        if not name.endswith(".txt"):
            continue
        base = name[: -len(".txt")]
        rs_path = os.path.join(rs_dir, base)
        if not os.path.exists(rs_path):
            issues.append(f"knapsack/{base}: does not exist in resources/knapsack")
            continue

        di = _no_blank(os.path.join(di_dir, name))
        n, cap = int(di[0]), int(di[1])
        weights, values = di[2].split(), di[3].split()
        if not (len(weights) == len(values) == n):
            issues.append(
                f"knapsack/{base}: n={n} but there are {len(weights)} weights and "
                f"{len(values)} values in DataInstances")
            continue

        rs = _no_blank(rs_path)
        rn, rcap = (int(x) for x in rs[0].split()[:2])
        items = [line.split() for line in rs[1:1 + n]]   # Rust stops at n items
        if len(items) < n:
            issues.append(
                f"knapsack/{base}: resources has {len(items)} items but {n} were expected")
            continue
        rs_values = [it[0] for it in items]
        rs_weights = [it[1] for it in items]

        if rn != n:
            issues.append(f"knapsack/{base}: n={n} in DataInstances vs {rn} in resources")
        if rcap != cap:
            issues.append(f"knapsack/{base}: capacity {cap} vs {rcap} in resources")
        if rs_weights != weights:
            issues.append(f"knapsack/{base}: the weights do not match resources")
        if rs_values != values:
            issues.append(f"knapsack/{base}: the values do not match resources")

    missing_di = sorted(
        set(os.listdir(rs_dir)) - {n[:-4] for n in os.listdir(di_dir) if n.endswith(".txt")})
    for base in missing_di:
        issues.append(f"knapsack/{base}: is in resources but not in DataInstances")
    return issues


def check_sequencing():
    """Same format on both sides: the files must be identical."""
    # Standard/, not Custom/: Custom/ holds the superseded generated instances.
    di_dir = os.path.join(DI, "Sequencing", "Standard")
    rs_dir = os.path.join(RS, "sequencing")
    issues = []

    di_names = {n for n in os.listdir(di_dir) if n.endswith(".txt")}
    rs_names = {n for n in os.listdir(rs_dir) if n.endswith(".txt")}
    for name in sorted(di_names - rs_names):
        issues.append(f"sequencing/{name}: does not exist in resources/sequencing")
    for name in sorted(rs_names - di_names):
        issues.append(f"sequencing/{name}: is in resources but not in DataInstances")
    for name in sorted(di_names & rs_names):
        if open(os.path.join(di_dir, name)).read() != open(os.path.join(rs_dir, name)).read():
            issues.append(f"sequencing/{name}: the content differs between the two sides")
    return issues


def main():
    issues = check_knapsack() + check_sequencing()
    if issues:
        print(f"❌ {len(issues)} inconsistency(ies) between DataInstances/ and resources/:")
        for p in issues:
            print(f"   - {p}")
        print("\nFix them before running: otherwise C++/Python and ddo solve "
              "different instances and the comparison of the paper is not valid.")
        return 1
    print("✅ knapsack and sequencing are consistent between DataInstances/ and resources/")
    return 0


if __name__ == "__main__":
    sys.exit(main())
