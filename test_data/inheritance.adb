with pack; use pack;

with ada.text_io;

-- pack.ads:  
--  package pack is 
--    X: Integer := 1;
--  end pack;

procedure main is 

   type X is tagged record 
      V: Integer := 1;
   end record;

   type Y is new X with record 
      W: Integer;
   end record;

begin

   ada.text_io.put_line(Integer'Image(1));

end main;
