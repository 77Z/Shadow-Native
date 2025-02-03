/*use lazy_static::lazy_static;

lazy_static! {
    static ref callback_function: extern fn() = extern {
        println!("Not yet initialized");
    };
}*/

#[no_mangle]
pub extern "C" fn register_function(callmeplz: extern fn()) {
    /*lazy_static! {
        static ref callback_function: extern fn() = || {
            println!("Different");
        };
    }*/
}

#[no_mangle]
pub extern "C" fn passthroughStart() {
    println!("Hello! -- from Rust!!!");
//    callback_function();
}
