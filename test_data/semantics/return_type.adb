with ada.text_io;

procedure main is 
   type type1 is Integer;

   function Func return type1 is
   begin
      return x + Y;
   end Func;  

begin

   ada.text_io.put_line(Integer'Image(Func(1)));
   ada.text_io.put_line(Integer'Image(Func(1, 2)));

end main;

-- 