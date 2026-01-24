procedure main is
   package pack is 
      type x is new Integer;
      l: pack.x;
   private
      type y is new Integer;
      v: pack.y;
   end pack;

   package body pack is 
      procedure proc is
         v: Integer;
      begin
         v := 1;
      end proc;
      type z is new Integer;
   end pack;

   qwer: pack.x;


begin
   qwer := 1;
end main;