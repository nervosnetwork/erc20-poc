extern crate tiny_keccak;
extern crate hex;
extern crate secp256k1;

use hex::{encode, decode};
use std::env;
use secp256k1::{Secp256k1, Message, SecretKey, PublicKey};
use tiny_keccak::Keccak;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Usage: {} <private key in hex string> <args>...", args[0]);
        return;
    }

    let private_key_string = &args[1];
    if private_key_string.len() != 64 {
        println!("Private key format error!");
        return;
    }
    let private_key = decode(private_key_string).unwrap();

    let secp = Secp256k1::signing_only();
    let key = SecretKey::from_slice(&secp, &private_key).unwrap();

    let public_key = PublicKey::from_secret_key(&secp, &key);

    let mut hasher = Keccak::new_sha3_256();
    for arg in args.iter().skip(2) {
        hasher.update(arg.as_bytes());
    }
    let mut output: [u8; 32] = [0; 32];
    hasher.finalize(&mut output);

    let mut hasher = Keccak::new_sha3_256();
    hasher.update(&output);
    let mut output: [u8; 32] = [0; 32];
    hasher.finalize(&mut output);

    let message = Message::from_slice(&output).unwrap();

    let signature = secp.sign(&message, &key);

    println!("Pubkey: {}", encode(&public_key.serialize()[..]));
    println!("Signature: {}", encode(signature.serialize_der(&secp)));
}
