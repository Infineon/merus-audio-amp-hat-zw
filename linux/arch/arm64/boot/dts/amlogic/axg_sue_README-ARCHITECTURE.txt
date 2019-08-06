DEVICE TREES FOR STREAM183x
===========================

axg_sue_s183x_internal.dtsi
-----------------------------

DT for components located on Stream183x module itself.


axg_sue_s183x_external_default.dtsi
-------------------------------------

DT for default configuration of Stream183x external IOs.


axg_sue_s183x_external_factory.dtsi
-------------------------------------

DT for factory configuration of Stream183x external IOs.

This file is empty for now, as all Stream183x outputs are by default GPIOs
which is exactly what we need.



DEVICE TREES FOR CARRIER BOARDS
===============================

axg_sue_prime.dtsi
------------------

DT for components located on StreamKit prime carrier board or daughter
boards which are not detectable via ADC.


axg_sue_factory.dtsi
--------------------
DT for components located on factory tester carrier board.

This file is empty for now, as there is nothing on the factory module, which
would require device tree entry.
