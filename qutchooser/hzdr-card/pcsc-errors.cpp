void PCSC_err2str(BYTE *sw) {
'6X XX' Transmission protocol related codes                                                                          
'61 XX' SW2 indicates the number of response bytes still available                                                   
'62 00' No information given                                                                                         
'62 81' Returned data may be corrupted                                                                               
'62 82' The end of the file has been reached before the end of reading                                               
'62 83' Invalid DF                                                                                                   
'62 84' Selected file is not valid. File descriptor error                                                            
'63 00' Authentification failed. Invalid secret code or forbidden value                                              
'63 81' File filled up by the last write                                                                             
'63 CX' Counter provided by 'X' (valued from 0 to 15) (exact meaning depending on the command)                       
'65 01' Memory failure. There have been problems in writing or reading the EEPROM.                                
'65 81' Write problem / Memory failure / Unknown mode                                                                
'67 XX' Error, incorrect parameter P3 (ISO code)                                                                     
'67 00' Incorrect length or address range error                                                                      
'68 00' The request function is not supported by the card.                                                           
'68 81' Logical channel not supported                                                                                
'68 82' Secure messaging not supported                                                                               
'69 00' No successful transaction executed during session                                                            
'69 81' Cannot select indicated file, command not compatible with file organization                                  
'69 82' Access conditions not fulfilled                                                                              
'69 83' Secret code locked                                                                                           
'69 84' Referenced data invalidated                                                                                  
'69 85' No currently selected EF, no command to monitor / no Transaction Manager File                                
'69 86' Command not allowed (no current EF)                                                                          
'69 87' Expected SM data objects missing                                                                             
'69 88' SM data objects incorrect                                                                                    
'6A 00' Bytes P1 and/or P2 are incorrect.                                                                            
'6A 80' The parameters in the data field are incorrect                                                               
'6A 81' Card is blocked or command not supported                                                                     
'6A 82' File not found                                                                                               
'6A 83' Record not found                                                                                             
'6A 84' There is insufficient memory space in record or file                                                         
'6A 85' Lc inconsistent with TLV structure                                                                           
'6A 86' Incorrect parameters P1-P2                                                                                   
'6A 87' The P3 value is not consistent with the P1 and P2 values.                                                    
'6A 88' Referenced data not found.                                                                                   
'6B 00' Incorrect reference; illegal address; Invalid P1 or P2 parameter                                             
'6C XX' Incorrect P3 length.                                                                                         
'6D 00' Command not allowed. Invalid instruction byte (INS)                                                          
'6E 00' Incorrect application (CLA parameter of a command)                                                           
'6F 00' Checking error                                                                                               
'90 00' Command executed without error                                                                               
'91 00' Purse Balance error cannot perform transaction                                                               
'91 02' Purse Balance error                                                                                          
'92 XX' Memory error                                                                                                 
'92 02' Write problem / Memory failure                                                                               
'92 40' Error, memory problem                                                                                        
'94 XX' File error                                                                                                   
'94 04' Purse selection error or invalid purse                                                                       
'94 06' Invalid purse detected during the replacement debit step                                                     
'94 08' Key file selection error                                                                                     
'98 XX' Security error                                                                                               
'98 00' Warning                                                                                                      
'98 04' Access authorization not fulfilled                                                                           
'98 06' Access authorization in Debit not fulfilled for the replacement debit step                                   
'98 20' No temporary transaction key established                                                                     
'98 34' Error, Update SSD order sequence not respected, (should be used if  SSD Update commands are received out of sequence).
'9F XX' Success, XX bytes of data available to be read via "Get_Response" task.                                      
