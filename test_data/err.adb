with ada.text_io; 

procedure Proc is 

   function foo return Integer is
   begin
      return 1;
   end foo;

   x: Integer := -foo;

begin 
   --  Put("Hi from ada");
   --  New_Line;
   --  X: Integer := 1;

   ada.text_io.put_line(Integer'Image(x));
   ada.text_io.put_line(Integer'Image(-foo));

end Proc;