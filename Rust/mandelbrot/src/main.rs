use rayon::prelude::*;
use std::env;
use std::fs::File;
use std::io::{BufWriter, Write};
use std::time::Instant;

const MAX_ITER: usize = 2048;
const NUM_RUNS: usize = 3;

fn mandelbrot(real: f64, imag: f64) -> usize {
    let (mut z_re, mut z_im) = (0.0, 0.0);
    for i in 0..MAX_ITER {
        if z_re * z_re + z_im * z_im > 4.0 {
            return i;
        }
        let new_re = z_re * z_re - z_im * z_im + real;
        let new_im = 2.0 * z_re * z_im + imag;
        z_re = new_re;
        z_im = new_im;
    }
    MAX_ITER
}

fn generate_image(size: usize, threads: usize, view: usize, save: bool) -> (f64, f64) {
    let mut image = vec![0usize; size * size];

    let (x0, y0, dx, dy) = match view {
        1 => (-2.5, -1.5, 3.5 / size as f64, 3.0 / size as f64),
        6 => (-1.4, 0.0, 0.002 / size as f64, 0.002 / size as f64),
        _ => (-2.0, -1.5, 3.0 / size as f64, 3.0 / size as f64),
    };

    let pool = rayon::ThreadPoolBuilder::new()
        .num_threads(threads)
        .build()
        .unwrap();

    let mut best_total = f64::MAX;
    let mut best_workload = f64::MAX;

    for _ in 0..NUM_RUNS {
        let total_start = Instant::now();
        let workload = pool.install(|| {
            let work_start = Instant::now();

            image
                .par_chunks_mut(size)
                .enumerate()
                .for_each(|(j, row)| {
                    for i in 0..size {
                        let real = x0 + i as f64 * dx;
                        let imag = y0 + j as f64 * dy;
                        row[i] = mandelbrot(real, imag);
                    }
                });

            work_start.elapsed().as_secs_f64() * 1000.0
        });
        let total = total_start.elapsed().as_secs_f64() * 1000.0;

        if total < best_total {
            best_total = total;
            best_workload = workload;
        }
    }

    if save {
        let filename = format!("mandelbrot_t{}_s{}_v{}.ppm", threads, size, view);
        let file = File::create(filename).unwrap();
        let mut writer = BufWriter::new(file);
        writeln!(writer, "P3\n{} {}\n255", size, size).unwrap();

        for j in 0..size {
            for i in 0..size {
                let val = (255.0 * image[j * size + i] as f64 / MAX_ITER as f64) as u8;
                writeln!(writer, "{} {} {}", val, val, val).unwrap();
            }
        }
    }

    (best_total, best_workload)
}


fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 4 {
        eprintln!("Usage: {} <threads> <size> <view> [save]", args[0]);
        std::process::exit(1);
    }

    let threads = args[1].parse::<usize>().unwrap();
    let size = args[2].parse::<usize>().unwrap();
    let view = args[3].parse::<usize>().unwrap();
    let save = args.get(4).map_or(false, |s| s == "1");

    let (total_ms, workload_ms) = generate_image(size, threads, view, save);
    let overhead_ms = total_ms - workload_ms;

    // Table output
    println!(
        "| {:<8} | {:<6} | {:<4} | {:<13.2} | {:<16.2} | {:<12.2} |",
        threads, size, view, total_ms, workload_ms, overhead_ms
    );

    // CSV output
    println!("{},{},{},{:.2},{:.2},{:.2}", threads, size, view, total_ms, workload_ms, overhead_ms);
}

