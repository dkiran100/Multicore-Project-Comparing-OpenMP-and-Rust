use rand::Rng;
use rayon::prelude::*;
use std::env;
use std::time::Instant;

const INF: i32 = 1_000_000_000;

fn generate_graph(n: usize, density: f32) -> Vec<Vec<i32>> {
    let mut dist = vec![vec![INF; n]; n];
    for i in 0..n {
        dist[i][i] = 0;
    }

    let max_edges = n * (n - 1);
    let edge_count = (density * max_edges as f32).round() as usize;
    let mut rng = rand::thread_rng();

    let mut added = 0;
    while added < edge_count {
        let u = rng.gen_range(0..n);
        let v = rng.gen_range(0..n);
        if u != v && dist[u][v] == INF {
            dist[u][v] = rng.gen_range(1..101);
            added += 1;
        }
    }

    dist
}

fn floyd_warshall(n: usize, dist: &mut Vec<Vec<i32>>) {
    for k in 0..n {
        let dist_k = dist[k].clone(); // Shared read-only row
        dist.par_iter_mut().for_each(|row_i| {
            for j in 0..n {
                if row_i[k] < INF && dist_k[j] < INF {
                    let new_dist = row_i[k] + dist_k[j];
                    if new_dist < row_i[j] {
                        row_i[j] = new_dist;
                    }
                }
            }
        });
    }
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 4 {
        eprintln!("Usage: {} <nodes> <density 0.0-1.0> <threads>", args[0]);
        return;
    }

    let n: usize = args[1].parse().expect("Invalid number of nodes");
    let density: f32 = args[2].parse().expect("Invalid density (0.0-1.0)");
    let num_threads: usize = args[3].parse().expect("Invalid thread count");

    rayon::ThreadPoolBuilder::new()
        .num_threads(num_threads)
        .build_global()
        .unwrap();

    let mut dist = generate_graph(n, density);

    let total_start = Instant::now();
    let (work_time, _) = rayon::ThreadPoolBuilder::new()
        .num_threads(num_threads)
        .build()
        .unwrap()
        .install(|| {
            let work_start = Instant::now();
            floyd_warshall(n, &mut dist);
            let work_end = Instant::now();
            (work_end.duration_since(work_start).as_secs_f64(), ())
        });

    let total_time = total_start.elapsed().as_secs_f64();
    let overhead = total_time - work_time;


    // Formatted table output
    println!(
        "| {:<6} | {:<7} | {:<7.2} | {:<13.6} | {:<16.6} | {:<10.6} |",
        n, num_threads, density, total_time, work_time, overhead
    );

    // CSV-style output
    println!("{},{},{:.2},{:.6},{:.6},{:.6}", n, num_threads, density, total_time, work_time, overhead);
}

