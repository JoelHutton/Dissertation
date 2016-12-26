<?php
    function sendSMS($content) {
        $ch = curl_init('https://api.smsbroadcast.co.uk/api-adv.php');
        curl_setopt($ch, CURLOPT_POST, true);
        curl_setopt($ch, CURLOPT_POSTFIELDS, $content);
        curl_setopt($ch, CURLOPT_RETURNTRANSFER, true);
        $output = curl_exec ($ch);
        curl_close ($ch);
        return $output;    
    }

    $username = 'underlyingfunk';
    $password = 'talligatorc3192';
    //$destination = '07434036331'; //Connie
    //$destination ='07765406607'; //Step
    //$source    = '07414939525'; //Ben
    //$source    = '07572689043'; //Jenny
    $source = 'NATHAN'; 
                    //Jenny, James, Ross, Angus, Patrick, Jonas, Suave
    //$destination = '07572689043,07729837696,07792676955,07578908062,07584429236,07443653895,07725476991,07502437786'; //Connie
    $destination = '07824333205'; 
    $text = "hello, it's Nathan from the future";
    $ref = 'ref1';
        
    $content =  'username='.rawurlencode($username).
                '&password='.rawurlencode($password).
                '&to='.rawurlencode($destination).
                '&from='.rawurlencode($source).
                '&message='.rawurlencode($text).
                '&ref='.rawurlencode($ref);
  
    $smsbroadcast_response = sendSMS($content);
    $response_lines = explode("\n", $smsbroadcast_response);
    
     foreach( $response_lines as $data_line){
        $message_data = "";
        $message_data = explode(':',$data_line);
        if($message_data[0] == "OK"){
            echo "The message to ".$message_data[1]." was successful, with reference ".$message_data[2]."\n";
        }elseif( $message_data[0] == "BAD" ){
            echo "The message to ".$message_data[1]." was NOT successful. Reason: ".$message_data[2]."\n";
        }elseif( $message_data[0] == "ERROR" ){
            echo "There was an error with this request. Reason: ".$message_data[1]."\n";
        }
    }
?>T
