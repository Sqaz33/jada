with ada.text_io;

-- pack.ads:  
--  package pack is 
--    X: Integer := 1;
--  end pack;

procedure main is 
   type type1 is tagged record 
      X: Integer;
   end record;

   type type2 is new type1 with record 
      Y: Integer;
   end record;

   type type3 is new type2 with record
      X: Float; -- err
   end record;


   l: Integer;
begin
   --  B(z);
   -- main.adb:50:04: left hand side of assignment must be a variable
   l := 1;
end main;

-- 