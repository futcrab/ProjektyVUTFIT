<?php
ini_set('display_errors', 'stderr');

//Odstrani komentare z precitaneho riadku kodu
function no_comments($line){
    $buff = explode("#", trim($line, "\n"), 2);
    return $buff[0];
}

//Rozdeli riadok kodu na slova, ktore ulozi do pola
function setWord_array($line){
    $line = explode(" ", $line);
    $line = array_filter($line);    //funkcia odstrani prazdne hodnoty v poli
    $line = array_values($line);    //funkcia preindexuje pole
    return $line;
}
//Funkcia kontroluje spravny zapis escape sekvencii v stringu
function check_escape($String){
    $index = 0;
    $size = strlen($String);
    
    do {
        $index = strpos($String, "\\", $index);
        if($index !== false){
            if($index+4 <= $size){
                if(!is_numeric($String[$index+1]) || !is_numeric($String[$index+2]) || !is_numeric($String[$index+3])){
                    return 0;
                }else{
                    $index++;
                }
            }else{
                return 0;
            }
        }
    } while($index !== false);

    return 1;
}

//Regex kontrola Var
function checkVar($String){
    return preg_match("/(^LF|^TF|^GF)@[a-zA-Z_\-$&%*!?][a-zA-Z_\-$&%*!?0-9]*/",$String);
}

//Regex kontrola Symb, ktora naviac vracia aj typ Symb
function checkSymb($String){
    if(checkVar($String)){
        return 1;
    }else{
        $isSymb = preg_match("/(^string|^bool|^int|^nill)@[.]*/",$String);
        if($isSymb){
            $type = explode("@", $String, 2);
            switch($type[0]){
                case "string":
                    if(check_escape($type[1])){
                        return 2;
                    }else{
                        return 0;
                    }
                case "bool":
                    if($type[1] == "true" || $type[1] == "false")
                        return 3;
                    else{
                        return 0;
                    }
                case "int":
                    if(preg_match("/[+-]?\d+/",$type[1])){
                        return 4;
                    }else{
                        return 0;
                    }
                case "nill":
                    if("nill" == $type[1]){
                        return 5;
                    }else{
                        return 0;
                    }
            }
        }else{
            return 0;
        }
    }
}

//Regex kontrola labelu
function checkLabel($String){
    return preg_match("/[a-zA-Z_\-$&%*!?][a-zA-Z_\-$&%*!?0-9]*/",$String);
}

//kontrola typu
function checkType($String){
    if($String == "string" || $String == "bool" || $String == "int"){
        return 1;
    }else{
        return 0;
    }
}

//funkcia na vypisanie chyby 23
function error_23($error){
    error_log("Error - (Arguments) Syntactic error or invalid usage on line \"".$error."\"");
    exit(23);
}

//Pomocna funkcia na generovanie instrukcie v domDokumente
function create_ins($opcode, $order_nmb, $dom){
    $ins = $dom->createElement('instruction');
    $attr1 = $dom->createAttribute('order');
    $attr1->value = $order_nmb;
    $attr2 = $dom->createAttribute('opcode');
    $attr2->value = $opcode;

    $ins->appendChild($attr1);
    $ins->appendChild($attr2);

    return $ins;
}

//Pomocna funkcia na generovanie argumentov v domDokumente
//Funkcia zaroven kontroluje spravnost zapisu a vie aj hadzat chybove hlasenia
function create_arg($type, $value, $arg_nmb, $dom, $line){
    switch($type){
        case "var":
            if(checkVar($value)){
                $arg = $dom->createElement('arg'.$arg_nmb,htmlspecialchars($value));
                $attr1 = $dom->createAttribute('type');
                $attr1->value = 'var';
                $arg->appendChild($attr1);
                return $arg; 
            }else{
                error_23($line);
            }
            break;

        case "symb":
            switch(checkSymb($value)){
                case 0:
                    error_23($line);
                    break;
                case 1:
                    $arg = $dom->createElement('arg'.$arg_nmb,htmlspecialchars($value));
                    $attr1 = $dom->createAttribute('type');
                    $attr1->value = 'var';
                    
                    $arg->appendChild($attr1);
                    return $arg;
                    break;
                default:
                    $arg_value = explode("@",$value,2);

                    $arg = $dom->createElement('arg'.$arg_nmb,htmlspecialchars($arg_value[1]));
                    $attr1 = $dom->createAttribute('type');
                    $attr1->value = $arg_value[0];
                    
                    $arg->appendChild($attr1);
                    return $arg;
                    break;
            }
            break;

        case "label":
            if(checkLabel($value)){
                $arg = $dom->createElement('arg'.$arg_nmb,htmlspecialchars($value));
                $attr1 = $dom->createAttribute('type');
                $attr1->value = 'label';
                $arg->appendChild($attr1);
                return $arg;
            }else{
                error_23($line);
            }
            break;

        case "type":
            if(checkType($value)){
                $arg = $dom->createElement('arg'.$arg_nmb,htmlspecialchars($value));
                $attr1 = $dom->createAttribute('type');
                $attr1->value = 'type';
                $arg->appendChild($attr1);
                return $arg;
            }else{
                error_23($line);
            }
            break;
        default:
            error_23($line);
            break;
    }
}
//Vypis --help pri spusteni programu
if($argc > 1){
    if($argv[1] == "--help" && $argc < 3){
        echo("Usage: php8.1 parse.php <SOURCE >OUTPUT\n");
        exit(0);
    }else{
        error_log("Error - Invalid combination of parameters (try --help)");
        exit(10);
    }
}
//Premenna na kontrolu prveho riadku
$first_line = true;

//Zaciatok domDocumentu
$dom = new DOMDocument('1.0', 'utf-8');
$dom->preserveWhiteSpace = false;
$dom->formatOutput = true;

//Vytvorenie elementu program
$program = $dom->createElement('program');
$lang_attr = $dom->createAttribute('language');
$lang_attr->value = 'IPPcode22';
$program->appendChild($lang_attr);

//Premenna na pocitanie poctu instrukcii
$order_nmb = 0;

while($line = fgets(STDIN))
{
    //Pripravenie riadku kodu na parsovanie
    $line = no_comments($line);
    $line_words = setWord_array($line);
    $line_size = sizeof($line_words);
    //V pripade ze sme na prvom riadku prebieha kontrola hlavicky
    if($first_line && $line_size){
        if($line_words[0] == ".IPPcode22"){
            $first_line = false;
        }else{
            error_log("Error - Missing or wrong header in code");
            exit(21);
        }
    //Ak riadok obsahuje aspon jedno slovo tak zacina parsovanie
    }else if($line_size){
        $upped_command = strtoupper($line_words[0]);    //Zabecpecuje case insensitive pisanie op. kodu
        //Switch rozdeluje postup podla op. kodu riadku
        //Prikazy su rozdelene podla poctu a typu argumentov 
        switch($upped_command){
            //var symb
            case "MOVE":
            case "INT2CHAR":
            case "STRLEN":
            case "TYPE":
                $order_nmb++;
                if($line_size == 3){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("var", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $arg2 = create_arg("symb", $line_words[2], "2", $dom, $line);
                    $ins->appendChild($arg2);
                }else{
                    error_23($line);
                }
                $program->appendChild($ins);
                break;
            
            //{}
            case "CREATEFRAME":
            case "PUSHFRAME":
            case "POPFRAME":
            case "RETURN":
            case "BREAK":
                $order_nmb++;
                if($line_size == 1){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;
            
            //var
            case "DEFVAR":
            case "POPS":
                $order_nmb++;
                if($line_size == 2){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("var", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;

            //label
            case "CALL":
            case "LABEL":
            case "JUMP":
                $order_nmb++;
                if($line_size == 2){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("label", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;

            //symb
            case "PUSHS":
            case "WRITE":
            case "EXIT":
            case "DPRINT":
                $order_nmb++;
                if($line_size == 2){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("symb", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;
            
            //var symb symb
            case "ADD":
            case "SUB":
            case "MUL":
            case "IDIV":
            case "LT":
            case "GT":
            case "EQ":
            case "AND":
            case "OR":
            case "NOT":
            case "STRI2INT":
            case "CONCAT":
            case "GETCHAR":
            case "SETCHAR":
                $order_nmb++;
                if($line_size == 4){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("var", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $arg2 = create_arg("symb", $line_words[2], "2", $dom, $line);
                    $ins->appendChild($arg2);

                    $arg3 = create_arg("symb", $line_words[3], "3", $dom, $line);
                    $ins->appendChild($arg3);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;
            
            //var type
            case "READ":
                $order_nmb++;
                if($line_size == 3){
                    $ins = create_ins("READ", $order_nmb, $dom);

                    $arg1 = create_arg("var", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $arg2 = create_arg("type", $line_words[2], "2", $dom, $line);
                    $ins->appendChild($arg2);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;
                
                //label symb symb
                case "JUMPIFEQ":
                case "JUMPIFNEQ":
                    $order_nmb++;
                if($line_size == 4){
                    $ins = create_ins($upped_command, $order_nmb, $dom);

                    $arg1 = create_arg("label", $line_words[1], "1", $dom, $line);
                    $ins->appendChild($arg1);

                    $arg2 = create_arg("symb", $line_words[2], "2", $dom, $line);
                    $ins->appendChild($arg2);

                    $arg3 = create_arg("symb", $line_words[3], "3", $dom, $line);
                    $ins->appendChild($arg3);

                    $program->appendChild($ins);
                }else{
                    error_23($line);
                }
                break;
                //Neznamy op. kod (Error 22)
                default:
                    error_log("Error - Unknow operator on line \"".$error."\"");
                    exit(22);
                break;

        }
    }
}
//Pripojenie elementu program do dokumentu a vypis
$dom->appendChild($program);
echo ($dom->saveXML());

exit(0)
?>