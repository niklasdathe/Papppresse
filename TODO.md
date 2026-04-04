# TODO

- Klassendiagramm: IActuatorDrive ist inkonsistent modelliert. Korrekt ist apply(cmd): bool; die veralteten Methoden extend(), retract(), stop() im Diagramm entfernen.
- State/Sequenz Rueckhub nach Erfolg: Top-Endstop-Pfad fuer RETURN_UP_SUCCESS explizit und konsistent darstellen, damit kein Hanger entsteht und die endstop_reached-Sequenz eindeutig passt.
- state diagram has faulty logic, as it moves from PRESS_DOWN direactly to ABORT_LATCH, as they have the same transition conditions.
- mqtt publish state is currently void. Maybe it should be bool, to ensure the state being published and recieved with republishing logic if no answer is recieved? Or keep it simple? Adjust in sequencediagram/ code to make consistent
- Change all sequence diagrams to use the uml 2.5.1 compliant lifeline boxes
- 
