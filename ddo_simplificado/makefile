.PHONY: build test clean \
        run_knapsack run_knapsack_relaxed run_knapsack_restricted \
        run_misp run_misp_relaxed run_misp_restricted \
        run_scp run_scp_relaxed run_scp_restricted \
        run_socknapsack run_socknapsack_relaxed run_socknapsack_restricted \
        run_scheduler run_scheduler_relaxed run_scheduler_restricted \
        experiment_knapsack experiment_misp experiment_scp experiment_socknapsack \
        experiment_scheduler

# Compila el proyecto
build:
	cargo build -p ddo

# Ejecuta los tests unitarios de la librería
test:
	cargo test -p ddo

# Limpia artefactos de compilación
clean:
	cargo clean

# ── Knapsack ──────────────────────────────────────────────────────────────────

run_knapsack:
	cd ddo && cargo run --example compare_knapsack -- \
		f1_l-d_kp_10_269 results_knapsack_quick.csv Exact 99999

run_knapsack_relaxed:
	cd ddo && cargo run --example compare_knapsack -- \
		f1_l-d_kp_10_269 results_knapsack_quick.csv Relaxed 200

run_knapsack_restricted:
	cd ddo && cargo run --example compare_knapsack -- \
		f1_l-d_kp_10_269 results_knapsack_quick.csv Restricted 200

# ── MISP ──────────────────────────────────────────────────────────────────────

run_misp:
	cd ddo && cargo run --example compare_misp -- \
		MANN_a9.clq results_misp_quick.csv Exact 99999

run_misp_relaxed:
	cd ddo && cargo run --example compare_misp -- \
		MANN_a9.clq results_misp_quick.csv Relaxed 100

run_misp_restricted:
	cd ddo && cargo run --example compare_misp -- \
		MANN_a9.clq results_misp_quick.csv Restricted 100

# ── Set Cover ─────────────────────────────────────────────────────────────────

run_scp:
	cd ddo && cargo run --example compare_scp -- \
		set_covering_n100_m63_d0.1_b_w38_seed1.txt results_scp_quick.csv Exact 99999

run_scp_relaxed:
	cd ddo && cargo run --example compare_scp -- \
		set_covering_n100_m63_d0.1_b_w38_seed1.txt results_scp_quick.csv Relaxed 20000

run_scp_restricted:
	cd ddo && cargo run --example compare_scp -- \
		set_covering_v6_r3_seed1.txt results_scp_quick.csv Restricted 200

# ── SOC Knapsack ──────────────────────────────────────────────────────────────

run_socknapsack:
	cd ddo && cargo run --example compare_socknapsack -- \
		knapsack_n5m2o1b5_0.txt results_socknapsack_quick.csv Exact 99999

run_socknapsack_relaxed:
	cd ddo && cargo run --example compare_socknapsack -- \
		knapsack_n5m2o1b5_0.txt results_socknapsack_quick.csv Relaxed 200

run_socknapsack_restricted:
	cd ddo && cargo run --example compare_socknapsack -- \
		knapsack_n5m2o1b5_0.txt results_socknapsack_quick.csv Restricted 200

# ── Scheduler ─────────────────────────────────────────────────────────────────

run_scheduler:
	cd ddo && cargo run --example compare_scheduler -- \
		scheduler_n5_s025_seed1.txt results_scheduler_quick.csv Exact 99999

run_scheduler_relaxed:
	cd ddo && cargo run --example compare_scheduler -- \
		scheduler_n5_s025_seed1.txt results_scheduler_quick.csv Relaxed 200

run_scheduler_restricted:
	cd ddo && cargo run --example compare_scheduler -- \
		scheduler_n5_s025_seed1.txt results_scheduler_quick.csv Restricted 200

# ── Experimentos completos (batch) ────────────────────────────────────────────

experiment_knapsack:
	cd ddo && bash run_knapsack.sh

experiment_misp:
	cd ddo && bash run_misp.sh

experiment_scp:
	cd ddo && bash run_scp.sh

experiment_socknapsack:
	cd ddo && bash run_socknapsack.sh

experiment_scheduler:
	cd ddo && bash run_scheduler.sh
