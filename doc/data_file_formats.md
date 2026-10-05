# Data File Formats

- There are the following data files:
    - characters.toml
    - organisation.toml
    - facilities.toml

## Notes

- IDs should be 1-based.
    - This is so that `0` can be used as an array-end marker/array element initialiser for arrays of IDs.

- In conversation trees, node IDs should be unique within each conversation, but need not be unique globally, since they are only searched/compared within their own conversation object.
