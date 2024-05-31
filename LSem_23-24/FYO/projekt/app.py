def wavelength_to_rgb(wavelength):
    """
    Convert a wavelength in nanometers to an RGB color.
    
    Args:
        wavelength (float): The wavelength in nanometers.
        
    Returns:
        tuple: A tuple containing the RGB color values, each in the range [0, 1].
    """
    if wavelength < 380 or wavelength > 750:
        raise ValueError("Wavelength must be between 380 and 750 nanometers.")
    
    if wavelength < 440:
        red = -(wavelength - 440) / (440 - 380)
        green = 0.0
        blue = 1.0
    elif wavelength < 490:
        red = 0.0
        green = (wavelength - 440) / (490 - 440)
        blue = 1.0
    elif wavelength < 510:
        red = 0.0
        green = 1.0
        blue = -(wavelength - 510) / (510 - 490)
    elif wavelength < 580:
        red = (wavelength - 510) / (580 - 510)
        green = 1.0
        blue = 0.0
    elif wavelength < 645:
        red = 1.0
        green = -(wavelength - 645) / (645 - 580)
        blue = 0.0
    else:
        red = 1.0
        green = 0.0
        blue = 0.0
    
    # Adjust intensities for visibility
    if wavelength < 420:
        factor = 0.3 + 0.7 * (wavelength - 380) / (420 - 380)
    elif wavelength < 645:
        factor = 1.0
    else:
        factor = 0.3 + 0.7 * (750 - wavelength) / (750 - 645)
    
    red *= factor
    green *= factor
    blue *= factor
    
    return red, green, blue

# Example usage:
wavelength = 400  # Example wavelength in nanometers
rgb = wavelength_to_rgb(wavelength)
print("RGB:", rgb)
