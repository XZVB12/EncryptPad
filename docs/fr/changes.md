<style type="text/css">
h2
{
    text-align: center;
}
</style>
## Notes de version ##

### Version 0.5.0.0

* Armure ASCII
* Algorithmes de chiffrement ajoutés : CAMELLIA128, CAMELLIA192, CAMELLIA256, TWOFISH
* Algorithmes de hachage ajoutés : SHA384, SHA512, SHA224
* Algorithme de compression ajouté : BZIP2
* Botan a été mis à jour vers la version 2.13
* Correctifs de traduction pour le menu contextuel
* Nouveau système de compilation Python porté de Botan
* Correctif de bogue : l’échec de MDC n’était pas signalé à l’ouverture des fichiers
* Correctif de bogue : le cas particulier dans lequel le dernier paquet partiel était inférieur à 5 octets n’était pas traité correctement
* Correctif de bogue : le paramètre --key-file-length était brisé dans l’interpréteur de ligne de commande

### Version 0.4.0.4

* Un logiciel portable devrait pouvoir enregistrer tous ses fichiers en un seul endroit amovible, tel qu’une clé USB. EncryptPad crée son fichier de paramètres dans `$HOME/.encryptpad`. Après ce changement, EncryptPad vérifiera la présence d’un répertoire `encryptpad_repository` dans le chemin de l’application et l’utilisera pour les paramètres et les fichiers clés.
* Botan a été mis à jour vers la version 2.7.
* Plog a été mis à jour vers la version 1.1.4.
* Traduction en lituanien

### Version 0.4.0.0

* Prise en charge de fichiers volumineux. EncryptPad peut maintenant traiter les images de disques, les sauvegardes et autres sortes de fichiers de plusieurs giga-octets.
* Une barre de progression et un bouton d’annulation dans la fenêtre de chiffrement de fichier.
* Réglage de la taille des onglets dans l’éditeur de texte.
* Botan a été mis à jour vers la version 2.3.
* STLplus a été mis à jour vers la version 3.15.
* OpenPGP a été déplacé vers sa propre bibliothèque libencryptmsg.
* Le format WAD utilisé pour les fichiers chiffrés avec l’option de fichier clé persistant et avec la double protection a changé. Les fichiers de ce type supérieurs à 64 kilo-octets ne sont pas rétrocompatibles et ne peuvent pas être ouverts avec 0.3.2.5. Cela ne s’applique pas aux fichiers gpg et epd à chiffrement simple et sans fichier clé persistant.

### Version 0.3.2.5

* Correctif de bogue : si un fichier EPD « phrase de passe seulement » déchiffré comprenait moins de 4 caractères, le contenu était ignoré et EncryptPad produisait un fichier vide. Cela se produisait, car EncryptPad s’attendait à trouver un marqueur IWAD de 4 caractères. Ces fichiers seront maintenant ouverts correctement.
* Correctif de bogue : si un fichier texte en clair était ouvert puis enregistré chiffré, les paramètres de chiffrement n’étaient pas réinitialisés aux valeurs par défaut, mais utilisaient les paramètres du dernier fichier chiffré.
* Correctif de bogue : le paramètre `file` de la commande `encryptpad` ne prenait pas en charge les caractères non ASCII. C’est pourquoi il était impossible d’ouvrir un fichier directement à partir de l’explorateur Windows par un double-clic si le fichier était dans un répertoire dont le nom comprenait des caractères non ASCII.
* Plus de propriétés dans les préférences afin de contrôler les paramètres de chiffrement par défaut : longueur de la séquence aléatoire du fichier clé, propriétés de chiffrement des fichiers clés, propriétés par défaut de chiffrement des fichiers (chiffre, S2K, itérations, compression), le nombre de clés de chiffrement à enregistrer ou à charger sans redemander la phrase de passe.
* Correctif de bogue : si plusieurs EncryptPad étaient ouverts et que les préférences étaient mises à jour, lors de la fermeture le dernier EncryptPad écrasait les préférences modifiées dans les autres EncryptPad.
* Dans la boîte de dialogue Chiffrement du fichier, un bouton radio a été ajouté afin de choisir entre EPD et GPG. L’utilisateur devait précédemment modifier l’extension manuellement pour obtenir une sortie au format GPG.
* Correctif de bogue : « Sel ;» était affiché dans la boîte de dialogue « Chiffrement du fichier » après le chiffrement ou le déchiffrement d’un fichier, ce qui n’était pas logique, car cela n’était pas relié au prochain chiffrement.
* Il n’y a que 256 ensembles d’itérations S2K possibles. La boîte de dialogue des propriétés de fichier propose maintenant une zone combinée pour les lister tous. Il est plus simple de choisir soit le nombre maximal, soit le nombre minimal d’itérations.
* Le nombre par défaut d’itérations a été porté à 1015808.
* La phrase de passe d’un fichier clé n’est plus demandée si le fichier clé est local et non chiffré, car cela était inutile.
* Correctif de bogue : après ouverture d’un fichier, le curseur représentait une flèche quand il survolait la zone de saisie de texte. Il représente maintenant `I` comme il se doit.
* Utilisation d’un seul exécutable EncryptPad pour toutes les langues. EncryptPad reconnaîtra la langue préférée du SE et l’utilisera pour la localisation. La gestion des versions en sera simplifiée, particulièrement pour les empaqueteurs.
* Argument `--lang` pour forcer une langue particulière au lieu de celle des paramètres régionaux du SE. Par exemple, `encryptpad --lang RU` pour charger les chaînes de localisation en russe.
* L’IUG d’EncryptPad a été traduite en chinois.
* Le fichier README.md a été traduit en français.
* STLplus a été mise à jour vers 3.13.


### Version 0.3.2.3

* Nombre configurable d’itérations S2K. Il peut être été fini par fichier, et comme valeur par défaut pour les nouveaux fichiers. Cette valeur est aussi utilisée pour la protection des fichiers.
* Prise en charge des octets d’estampille temporelle dans les fichiers GPG.
* L’exécutable a été renommé `encryptpad` (minuscules) sauf sur macOS.
* Amélioration de la cohérence de l’anglais dans l’IUG. Tous les « mot de passe » sont devenus des « phrase de passe ».
* Traduction en russe.
* Traduction en français.
* Ajout de l’algorithme de hachage `SHA-512`.
* Interpréteur de ligne de commande : encryptcli plantait avec une erreur de segmentation quand TTY n’était pas disponible. Il se ferme maintenant élégamment avec un code d’erreur.
* Correctif de bogue : dans la boîte de dialogue Chiffrement du fichier, quand la phrase de passe était effacée puis redéfinie, la phrase de passe était ignorée et le fichier était enregistré comme « clé seulement ».
* Correctif de bogue : dans le chiffrement des fichiers, les fichiers gpg étaient chiffrés avec le drapeau 't', ce qui entraînait la suppression de 0x0D octets quand GPG déchiffrait les fichiers, car il croyait que les fichiers étaient textuels. Le problème ne se produisait pas sous Windows ni quand EncryptPad était utilisé pour le déchiffrement. Cela ne se produisait que dans la direction EncryptPad vers GPG pour Linux/Unix.

### Version 0.3.2.2

* Correctif de bogue : dans la boîte de dialogue Chiffrement du fichier, lorsque la phrase de passe du fichier clé était erronée, un message n’en informait pas l’utilisateur.
* Prise en charge de fichiers bak. Lors d’une modification, un fichier .bak n’est crée que lors du premier enregistrement.
* L’ordre des sections a été changé dans les nouveaux fichiers wad. Il est maintenant répertoire, clé, contenu. Cela n’affecte pas les fichiers wad existants.
* encryptcli peut lire les phrases de passe des descripteurs de fichier
* encryptcli peut lire de stdin et écrire vers stdout
* Les arguments d’encryptcli ont été revus et améliorés
* Botan a été mis à jour vers la version 1.10.12
* Version 64 bits
* Correctif des droits de compilation
* La compilation peut maintenant fonctionner avec g++ sous Linux
* La compilation peut maintenant utiliser les bibliothèques système sous Linux incluant Zlib et Botan
* Correctif de la compilation sous Fedora 23
* Correctif des avertissements

### Version 0.3.2.1

* Première version à code source ouvert
