use rand::Rng;
use rand::SeedableRng;
use rand::rngs::StdRng;
use rayon::ThreadPoolBuilder;
use rayon::join;
use std::env;
use std::time::Instant;

fn rand_num_array(size: usize) -> Vec<i32> {
    let seed: u64 = 40;
    let mut rng = StdRng::seed_from_u64(seed);
    (0..size).map(|_| rng.gen_range(0..1_000_000)).collect()
}

fn quicksort_parallel(arr: &mut [i32], depth: usize, max_depth: usize) {
    if arr.len() <= 1 {
        return;
    }

    let pivot_index = partition(arr);
    let (left, right) = arr.split_at_mut(pivot_index);

    if depth < max_depth {
        join(
            || quicksort_parallel(left, depth + 1, max_depth),
            || quicksort_parallel(&mut right[1..], depth + 1, max_depth), // skip pivot
        );
    } else {
        quicksort_parallel(left, depth + 1, max_depth);
        quicksort_parallel(&mut right[1..], depth + 1, max_depth);
    }
}

fn partition(arr: &mut [i32]) -> usize {
    let len = arr.len();
    let pivot_index = len / 2;
    arr.swap(pivot_index, len - 1);
    let pivot = arr[len - 1];

    let mut i = 0;
    for j in 0..len - 1 {
        if arr[j] < pivot {
            arr.swap(i, j);
            i += 1;
        }
    }

    arr.swap(i, len - 1);
    i
}

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() != 3 {
        eprintln!(
            "Usage: {} <threads> <array_size>",
            args[0]
        );
        std::process::exit(1);
    }

    let num_threads: usize = args[1].parse().expect("Invalid number of threads");
    let size: usize = args[2].parse().expect("Invalid number of elements");

    let mut array = rand_num_array(size);

    let pool = ThreadPoolBuilder::new()
        .num_threads(num_threads)
        .build()
        .expect("Failed to create thread pool");

    let max_depth = (num_threads as f64).log2().ceil() as usize;

    let total_start = Instant::now();
    let (work_time, _) = pool.install(|| {
        let work_start = Instant::now();
        quicksort_parallel(&mut array, 0, max_depth);
        let work_end = Instant::now();
        (work_end.duration_since(work_start).as_secs_f64(), ())
    });
    let total_time = total_start.elapsed().as_secs_f64();
    let overhead = total_time - work_time;

    // Formatted table output
    println!(
        "| {:<10} | {:<8} | {:<13.6} | {:<16.6} | {:<10.6} |",
        size, num_threads, total_time, work_time, overhead
    );

    // CSV-style output
    println!("{},{},{:.6},{:.6},{:.6}", size, num_threads, total_time, work_time, overhead);
}

