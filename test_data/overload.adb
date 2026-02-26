--  with pack; use pack;

with ada.text_io;
--  use ada.text_io;

procedure main is 


   package pack is
      function Func(X: Integer; Y: Integer) return Integer;

   private
      function Func(X: Integer) return Integer; 

      procedure Func(X: Integer);
   end pack;
   package body pack is
      function Func(X: Integer; Y: Integer) return Integer is
      begin
         return x + Y;
      end Func;  

      function Func(X: Integer) return Integer is
      begin
         return x + 1;
      end Func; 

      procedure Func(X: Integer) is
      begin
         return;
      end Func; 
   end pack;
   
begin

   --  ada.text_io.put_line(Integer'Image(pack.Func(1)));
   ada.text_io.put_line(Integer'Image(pack.Func(1, 2)));


end main;

-- 