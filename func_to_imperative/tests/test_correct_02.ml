
let a : int = 4 in
let a : string =
    let a : string = "qw" in
    a ^ a
in a ^ " " ^ a;;
let a : int = 2;;
let a : int = 3;;


let a : int = 4 in
let a : string =
    let a : string = "qw" in
    a ^ a
in a ^ " " ^ a;;

let c : int = 2;;

let int_min : int -> int -> int = fun a b ->
    if (a <= b) then
        a
    else
        b
;;

let int_max : int -> int -> int = fun a b ->
    if (a >= b) then
        a
    else
        b
;;

let a : int = 2;;

let rec a : int -> int -> int = fun b c ->
    if ((int_min b c) = 0) then
        b + c
    else
        let b_1 : int = int_min b c in
        let c_1 : int = int_max b c in
        a (c_1 mod b_1) b_1
;;

let arg_1 : int = read_int ();;
let arg_2 : int = read_int ();;

print_string ((string_of_int (a arg_1 arg_2)) ^ "\n");;

