let rec gcd : int -> int -> int = fun a b ->
	if (a < b) then
		gcd b a
	else
		if (b = 0) then
			a
		else
			gcd b (a mod b)
;;

let is_prime : int -> bool = fun p ->
    let rec loc_is_prime : int -> int -> bool = fun p c ->
        if (c * c > p) then
            true
        else
            if (p mod c = 0) then
                false
            else
                loc_is_prime p (c + 1)
    in
    loc_is_prime p 2
;;

let string_of_bool : bool -> string = fun b ->
    if (b) then
        "true"
    else
        "false"
;;

let a : int = read_int ();;
let b : int = read_int ();;

print_int (gcd a b);;

print_string ("\n" ^ (string_of_bool (is_prime a)) ^ " " ^ (string_of_bool (is_prime b)) ^ "\n");;

read_int ();;

