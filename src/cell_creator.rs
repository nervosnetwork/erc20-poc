extern crate flatbuffers;

#[path = "./erc20_generated.rs"]
mod erc20_generated;
pub use erc20_generated::e_r_c_2_0::sample::{Balance, Allowed, Data,
                                             BalanceArgs, AllowedArgs, DataArgs};

use std::env;
use std::fs::File;
use std::io::{self, Write, BufRead};
use std::str::FromStr;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Usage: {} <output file path>", args[0]);
        return;
    }

    let mut builder = flatbuffers::FlatBufferBuilder::new_with_capacity(1024);

    let stdin = io::stdin();
    let mut iterator = stdin.lock().lines();

    println!("Enter owner address:");
    let buffer = iterator.next().unwrap().unwrap();
    if buffer.is_empty() {
        println!("Exiting...");
        return;
    }
    let owner = builder.create_string(&buffer);

    println!("Enter total supply:");
    let buffer = iterator.next().unwrap().unwrap();
    if buffer.is_empty() {
        println!("Exiting...");
        return;
    }
    let total_supply = u64::from_str(&buffer).unwrap();

    let mut balances = Vec::new();
    loop {
        println!("Enter balance address(empty string to finish entering):");
        let buffer = iterator.next().unwrap().unwrap();
        if buffer.is_empty() {
            break;
        }
        let address = builder.create_string(&buffer);

        println!("Enter address tokens:");
        let buffer = iterator.next().unwrap().unwrap();
        if buffer.is_empty() {
            println!("Exiting...");
            return;
        }
        let tokens = u64::from_str(&buffer).unwrap();

        let balance = Balance::create(&mut builder, &BalanceArgs{
            address: Some(address),
            tokens,
        });
        balances.push(balance);
    }

    let mut allowed = Vec::new();
    loop {
        println!("Enter allowed address(empty string to finish entering):");
        let buffer = iterator.next().unwrap().unwrap();
        if buffer.is_empty() {
            break;
        }
        let address = builder.create_string(&buffer);

        println!("Enter spender address:");
        let buffer = iterator.next().unwrap().unwrap();
        if buffer.is_empty() {
            println!("Exiting...");
            return;
        }
        let spender = builder.create_string(&buffer);

        println!("Enter allowed tokens:");
        let buffer = iterator.next().unwrap().unwrap();
        if buffer.is_empty() {
            println!("Exiting...");
            return;
        }
        let tokens = u64::from_str(&buffer).unwrap();

        let a = Allowed::create(&mut builder, &AllowedArgs{
            address: Some(address),
            spender: Some(spender),
            tokens,
        });
        allowed.push(a);
    }

    let balances_vec = builder.create_vector(&balances);
    let allowed_vec = builder.create_vector(&allowed);
    let data = Data::create(&mut builder, &DataArgs{
        owner: Some(owner),
        new_owner: None,
        total_supply: total_supply,
        balances: Some(balances_vec),
        allowed: Some(allowed_vec),
    });

    builder.finish(data, None);

    let buf = builder.finished_data();

    let mut file = File::create(&args[1]).unwrap();
    file.write(buf).unwrap();
}
