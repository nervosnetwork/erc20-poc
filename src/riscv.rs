extern crate ckb_riscv;

use ckb_riscv::run;
use std::env;
use std::fs::File;
use std::io::Read;
use std::time::Instant;

fn main() {
    let args: Vec<String> = env::args().skip(1).collect();

    let mut file = File::open(args[0].clone()).unwrap();
    let mut buffer = Vec::new();
    file.read_to_end(&mut buffer).unwrap();

    let start = Instant::now();
    let result = run(&buffer, &args);
    let end = Instant::now();

    println!("Result: {:?}", result);
    println!("Running time: {:?}", end.duration_since(start));
}
