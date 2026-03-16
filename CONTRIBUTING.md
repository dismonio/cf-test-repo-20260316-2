# Contributing to Cyber Fidget

We welcome contributions to the Cyber Fidget firmware. Here's what you need to know.

## License

By submitting a pull request, you agree that your contribution is licensed under the same terms as the project: **GPL-3.0-or-later with the Cyber Fidget HAL Linking Exception** (see [LICENSE](LICENSE)).

You retain copyright over your contributions.

## Developer Certificate of Origin (DCO)

All commits must be signed off to certify you have the right to submit the code:

```
git commit -s -m "Your commit message"
```

This adds a `Signed-off-by: Your Name <your@email.com>` line, indicating you agree to the [DCO](https://developercertificate.org/):

> By making a contribution to this project, I certify that I have the right to submit it under the open source license indicated in the file.

## Getting Started

1. Fork the repository
2. Create a feature branch (`git checkout -b my-feature`)
3. Make your changes
4. Test on hardware or the WASM emulator
5. Commit with sign-off (`git commit -s`)
6. Push and open a pull request

## Guidelines

- Follow existing code style and conventions
- Apps should interact only through the HAL API (see [PERMISSIONS.md](PERMISSIONS.md))
- Keep pull requests focused — one feature or fix per PR
- Include a brief description of what your change does and why

## Reporting Issues

Use [GitHub Issues](https://github.com/CyberFidget/cyberfidget-firmware/issues) for bugs and feature requests.

## Questions?

**support@dismoindustries.com**
