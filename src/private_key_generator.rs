extern crate rand;
extern crate hex;

use hex::encode;
use rand::{thread_rng, Rng};

fn main() {
    let mut arr = [0u8; 32];
    thread_rng().fill(&mut arr[..]);

    println!("{}", encode(arr));
}
