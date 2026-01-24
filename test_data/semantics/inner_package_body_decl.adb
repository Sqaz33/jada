procedure main is
   --  type xtype is record
   --     q: Integer;
   --  end record;

   package pack is 
      package packInner is
         procedure procInner;
      end packInner;

      procedure proc;
   end pack;

   package body pack is 
      procedure proc is
         v: Integer;
      begin
         v := 1;
      end proc;

      package body packInner is
         procedure procInner is 
            q: Integer;
         begin
            q := 1;
         end procInner;
      end packInner;
   end pack;

begin
   err;
end main;