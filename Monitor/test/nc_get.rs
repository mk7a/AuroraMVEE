use std::io::{Read, Result};
use std::net::TcpStream;

fn main() -> Result<()> {
    let mut stream = TcpStream::connect("localhost:4444")?;

    let mut buffer = [0; 4096];

    loop {
        let bytes_read = stream.read(&mut buffer)?;

        if bytes_read == 0 {
            println!("Connection closed.");
            break;
        }

        if let Some(_) = buffer[..bytes_read].iter().position(|&b| b == 0x0a) {
            println!("Newline 0x0a byte encountered, disconnecting.");
            break;
        }
    }

    Ok(())
}
// use std::io::{BufRead, BufReader};
// use std::net::TcpStream;
//
// fn main() {
//     let host = "localhost";
//     let port = 4444;
//
//     match TcpStream::connect((host, port)) {
//         Ok(stream) => {
//             let mut buf_reader = BufReader::new(stream);
//             let mut buffer = String::new();
//
//             match buf_reader.read_line(&mut buffer) {
//                 Ok(_) => println!("Data Read"),
//                 Err(e) => eprintln!("Error: {}", e),
//             }
//         }
//         Err(_) => eprintln!("Error connecting to {}:{}", host, port),
//     }
// }