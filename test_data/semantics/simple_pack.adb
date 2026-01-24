procedure main is

   package pack is 
      procedure proc;
   end pack;

   package body pack is 
      procedure proc is
         v: Integer;
      begin
         v := 1;
      end proc;
   end pack;


begin
   pack.proc;
end main;