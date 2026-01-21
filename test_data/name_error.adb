--  with pack; use pack;

with ada.text_io;

-- pack.ads:  
--  package pack is 
--    X: Integer := 1;
--  end pack;

procedure main is 
   type x is record 
      X: Integer;
   end record;

   package a is
      procedure z is
         Y: Integer;
      begin
         Y := 1;
      end z;

      function y return x is 
         l: X; -- если назвать перм x - ошибка
      begin 
         l.X := 1;
         return l;
      end y;

      --  procedure x is --err
      --  begin
      --     1; 
      --  end x;

      v: x;
   end a;

begin

   ada.text_io.put_line(Integer'Image(1));

end main;
