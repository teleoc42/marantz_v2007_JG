# Composant ESPHome marantz_v2007

Composant externe ESPHome pour piloter un ampli Marantz SR5004 (et
compatibles : SR7002, SR8002, SR6003, SR7003, SR8003, SR6004, AV7005,
AV8003) via le port RS232, protocole "legacy" `@CMD:VALEUR\r`.

## État actuel

Fonctionne (basé sur le polling déjà testé) :
- Lecture de l'état : power, volume, mute, source (toutes les 5s)
- Allumage / extinction / toggle
- Mute / unmute
- Volume haut / bas (pas à pas)
- Sélection de source par nom (TV, CD, TUNER, M-XPORT, BLU-RAY, DVD,
  VCR, VINYL, AUX)

À vérifier / compléter :
- Les commandes d'écriture (`@PWR:2`, `@AMT:2`, `@VOL:1`...) sont
  déduites par symétrie avec les réponses déjà validées, et
  confirmées par la documentation publique de ce protocole. Elles ne
  sont pas encore confirmées avec CET ampli précis. Utilise le champ
  "Marantz - commande brute" du fichier `exemple_configuration.yaml`
  pour tester et ajuster si besoin.
- Réglage du volume en absolu (slider) : pas encore implémenté, car
  on ne connait pas la plage exacte en dB de cet ampli. Une fois que
  tu auras confirmé la plage réelle (avec la commande brute), on
  pourra l'activer.

## Fichiers supprimés depuis la version précédente

`protocol.h/.cpp`, `parser.h/.cpp` (vides, jamais utilisés),
`sensor.py`, `switch.py` (vides, non référencés), `manifest.json`
(concept Home Assistant, pas ESPHome), `const.py` (non utilisé).
