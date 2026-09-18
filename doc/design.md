# Design

## Data Model


### Organisation Data Model

- Foundation
  - budget
  - mtfs
  - scps
  - reputation
    - high/medium/low
- SCP
  - ID
  - name
  - object class
  - description
- MTF
  - name
  - member count
    - operational
    - wounded
    - dead
  - specialism

### Rooms Data Model

- Room
  - name
  - ID
  - description
  - characters
  - objects
  - connections
- Facility
  - rooms
  - entrances
- Containment
  - ID
  - name
  - scp ID
  - description

### Character Data Model

- Character
  - ID
  - name
  - description
  - role
  - conversations

- Conversation
  - ID
  - nodes
    - ID
    - text
    - options
- ConversationOption
  - text
  - links to

## Components


