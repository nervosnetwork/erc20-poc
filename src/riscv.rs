extern crate ckb_vm;

use ckb_vm::{run, SparseMemory};
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
    let args2: Vec<Vec<u8>> = args.iter().map(|a| a.clone().into_bytes()).collect();
    let result = run::<u64, SparseMemory>(&buffer, &args2);
    let end = Instant::now();

    println!("Result: {:?}", result);
    println!("Running time: {:?}", end.duration_since(start));
}
