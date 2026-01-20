--  with pack; use pack;

with ada.text_io;

procedure main is 
   function Func(X: Integer; Y: Integer) return Integer is
   begin
      return x + Y;
   end Func;  

   function Func(X: Integer) return Integer is
   begin
      return x + 1;
   end Func; 

begin

   ada.text_io.put_line(Integer'Image(Func(1)));
   ada.text_io.put_line(Integer'Image(Func(1, 2)));

end main;

-- 