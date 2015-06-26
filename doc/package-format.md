= MPK Package Format

== Packge Metadata

The package metadata includes information the package manager and the user needs
about a package. All metadata is stored the package database and created at the
time a revision of a package is published. 

Each packge hase a unique **package name**. The name must not change between 
different revisions of the same package. 

The **revision** number idendifies the exact version of the package. This is a 
64-bit unsigned integer that is incremented for each new release of the 
package. The revision number 0 has a special meaning: It identifies an empty 
package and is available in each release-set that contains the package. 
Installing revision 0 means uninstalling the package. The revision number is
defined by the package management server at the time a new revision of a 
package is added. The revision number has nothing to do with a software version 
defined by the developer of the package. 

Each package is build for a specific **architecture**. This has to match the
architecture named in the client features. 

A package can define packages it depends on and packages that must not be on the
system at the same time. For this the lists **depends** and **conflicts**n are
used. Each item of these lists contains a package name and a list of revisions
or revision ranges. 

The location where the package can be copied from is defined by the **source** 
metadata. 

The package **vendor** metadata contains name and contact information about a 
person resonsible for the package. 

=== Fields

**Package**

| field name | type               | (max.) size |
-------------------------------------------------
| name       | string             | 128         |
| revisions  | list of references |             |


**Revisions**

| field name | type               | (max.) size |
-------------------------------------------------
| package    | reference          |             |
| revision   | integer            | 8           |
| release    | reference          |             |
| depends    | list of references |             |
| conflicts  | list of references |             |
| source     | string             | 256         |

