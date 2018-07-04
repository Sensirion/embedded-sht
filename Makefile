drivers=sht3x shtc1
clean_drivers=$(foreach d, $(drivers), clean_$(d))
release_drivers=$(foreach d, $(drivers), release/$(d))

.PHONY: FORCE all $(release_drivers) $(clean_drivers)

all: $(drivers)

$(drivers): sht-common/git_version.c FORCE
	cd $@ && $(MAKE) $(MFLAGS)

sht-common/git_version.c: FORCE
	[ -d ".git" ] && git describe --always --dirty | \
		awk 'BEGIN \
		{print "/* THIS FILE IS AUTOGENERATED */"} \
		{print "#include \"git_version.h\""} \
		{print "const char * SHT_DRV_VERSION_STR = \"" $$0"\";"} \
		END {}' > $@ || echo "Can't update version, not a git repository"


$(release_drivers): sht-common/git_version.c
	export rel=$@ && \
	export driver=$${rel#release/} && \
	export tag="$$(git describe --always --dirty)" && \
	export pkgname="$${driver}-$${tag}" && \
	export pkgdir="release/$${pkgname}" && \
	rm -rf "$${pkgdir}" && mkdir -p "$${pkgdir}" && \
	cp -r embedded-common/* "$${pkgdir}" && \
	cp -r sht-common/* "$${pkgdir}" && \
	cp -r $${driver}/* "$${pkgdir}" && \
	perl -pi -e 's/^sensirion_common_dir :=.*$$/sensirion_common_dir := ./' "$${pkgdir}/Makefile" && \
	perl -pi -e 's/^sht_common_dir :=.*$$/sht_common_dir := ./' "$${pkgdir}/Makefile" && \
	cd "$${pkgdir}" && $(MAKE) $(MFLAGS) && $(MAKE) clean $(MFLAGS) && cd - && \
	cd release && zip -r "$${pkgname}.zip" "$${pkgname}" && cd - && \
	ln -sf $${pkgname} $@

release: clean $(release_drivers)

$(clean_drivers):
	export rel=$@ && \
	export driver=$${rel#clean_} && \
	cd $${driver} && $(MAKE) clean $(MFLAGS) && cd -

clean: $(clean_drivers)
	rm -rf release

