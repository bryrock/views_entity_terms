# views_entity_terms
--------------------
Extend the Views Taxonomy tid Contextual Filter plugin (default argument) to find terms referenced by entityreference.

What it does
------------

It's becoming more common (and it's totally valid) to add taxonomy fields to Drupal Content Types as entityreference field types, rather than using the traditonal term_reference field type.  A caveat to doing this, however, is the loss of an easy way in Views to find term arguments (Contextual Filters) from a current node.

The Contextual Filter "Taxonomy term: Term ID" can provide one or more default term values from a loaded node by using the default value type "Taxonomy term ID from URL" when the URL is a node, but this only works when the terms are stored in a term_reference field.  That's because the handler for that argument doesn't check anywhere else in the node.  It only checks term_reference fields.

This module extends that default argument plugin to provide and option to also check for terms stored in entityreference fields.  If terms (tids) are found, all other limits and/or vocabulary options work the same as they do when the tids are located in term_reference fields.

Installation
------------
Just put this module in your site's modules folder, then enable.

Requirements
------------
Requires EntityReference module (because without that you won't have any entityreference fields).
