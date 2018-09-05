use std::env;
use std::fs::File;
use std::io::Write;

fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() < 2 {
        println!("Usage: {} <output cell directory> <args>...", args[0]);
        return;
    }

    let path = &args[1];

    let mut file = File::create(format!("{}/argc", path)).unwrap();
    let content = format!("{}", args.len() - 2);
    file.write(&content.into_bytes()).unwrap();

    for (i, arg) in args.iter().skip(2).enumerate() {
        let mut file = File::create(format!("{}/argv{}", path, i)).unwrap();
        let content = format!("{}", arg);
        file.write(&content.into_bytes()).unwrap();
    }
}
