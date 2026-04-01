# TODO

- Klassendiagramm: IActuatorDrive ist inkonsistent modelliert. Korrekt ist apply(cmd): bool; die veralteten Methoden extend(), retract(), stop() im Diagramm entfernen.
- Sequenzdiagramm mqtt_state_publish: Rueckgabe ok : bool nach publishState(state) entfernen; 
- State/Sequenz Rueckhub nach Erfolg: Top-Endstop-Pfad fuer RETURN_UP_SUCCESS explizit und konsistent darstellen, damit kein Hanger entsteht und die endstop_reached-Sequenz eindeutig passt.
- state diagram has faulty logic, as it moves from PRESS_DOWN direactly to ABORT_LATCH, as they have the same transition conditions.
- mqtt publish state is currently void. Maybe it should be bool, to ensure the state being published and recieved with republishing logic if no answer is recieved? Or keep it simple?
