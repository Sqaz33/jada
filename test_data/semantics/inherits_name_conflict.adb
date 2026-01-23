with ada.text_io;

procedure main is 
   type base is tagged record 
      X: Integer;
   end record;

   type derived is new base with record 
      Y: Integer;
   end record;

   type derivedDerived is new derived with record 
      Y: Integer;
      X: Integer;
   end record;

   l: Integer;
begin
   l :=  1;
end main;

-- 