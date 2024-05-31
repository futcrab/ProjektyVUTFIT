import sys
from PyQt5.QtWidgets import QApplication, QWidget, QLabel, QSlider, QMainWindow, QVBoxLayout, QGridLayout, QSpacerItem, QSizePolicy, QComboBox
from PyQt5.QtGui import QPainter, QPen, QColor
from PyQt5.QtCore import Qt
from math import sqrt, asin, degrees
import numpy as np

nValuesOfGlass = [1.46, 1.505, 1.522, 1.569, 1.67, 1.728]
HValuesOfGlass = [3540, 4200, 4590, 5310, 7430, 13420]

class CentralWidget(QWidget):
    def __init__(self):
        super().__init__()

        self.setStyleSheet("background-color: black; color: white")
        self.diameter = 900
        self.sdiameter = 900
        self.nOfRays = 5
        self.indexOfGlass = 0
        self.aberr = 0
        self.angleAlpha = 5
        self.firstRayVL = 380
        self.secondRayVL = 750

        self.mainLayout = QGridLayout()
        layoutRight = QVBoxLayout()
        layoutLeft = QVBoxLayout()
        layoutRays = QVBoxLayout()
        layoutType = QVBoxLayout()
        layoutAberr = QVBoxLayout()
        self.layoutSph = QVBoxLayout()
        self.layoutComa1 = QVBoxLayout()
        self.layoutComa2 = QVBoxLayout()
        self.layoutComa3 = QVBoxLayout()
        self.layoutChroma1 = QVBoxLayout()
        self.layoutChroma2 = QVBoxLayout()
        self.layoutChroma3 = QVBoxLayout()
        self.layoutFocLen = QVBoxLayout()

        #Basic sliders
        self.label1 = self.createLabel("RightRadius(cm): 3.000")
        layoutRight.addWidget(self.label1)

        self.rightSlider = self.createHorizontalSlider(1750, 16000, 1000, 3000)
        self.rightSlider.valueChanged.connect(self.right_slider_value_changed)
        layoutRight.addWidget(self.rightSlider)

        self.label2 = self.createLabel("LeftRadius(cm): 3.000")
        layoutLeft.addWidget(self.label2)

        self.leftSlider = self.createHorizontalSlider(1750, 16000, 1000, 3000)
        self.leftSlider.valueChanged.connect(self.left_slider_value_changed)
        layoutLeft.addWidget(self.leftSlider)

        self.label3 = self.createLabel("# of Rays: 5")
        layoutRays.addWidget(self.label3)

        self.raySlider = self.createHorizontalSlider(3, 25, 1, 5)
        self.raySlider.valueChanged.connect(self.ray_slider_value_changed)
        layoutRays.addWidget(self.raySlider)

        self.label4 = self.createLabel("Material of Lens: ")
        layoutType.addWidget(self.label4)

        self.comboboxType = self.create_combobox(["Silica glass (n≈1.46)", "Borosilicate glass BK7 (n≈1.505)", "Hard crown glass K5 (n≈1.522)", "Barium crown glass BaK4 (n≈1.569)", "Barium flint glass BaF10 (n≈1.67)", "Dense flint glass SF10 (n≈1.728)"])
        self.comboboxType.setStyleSheet("font-size: 16px;")
        self.comboboxType.currentIndexChanged.connect(self.type_glass_value_changed)
        layoutType.addWidget(self.comboboxType)

        self.label5 = self.createLabel("Type of aberration: ")
        layoutAberr.addWidget(self.label5)

        self.comboboxAberr = self.create_combobox(["Spherical", "Coma", "Chromatic"])
        self.comboboxAberr.setStyleSheet("font-size: 16px;")
        self.comboboxAberr.currentIndexChanged.connect(self.aberr_value_changed)
        layoutAberr.addWidget(self.comboboxAberr)

        focal_len = self.calculate_focal_length(nValuesOfGlass[self.indexOfGlass], self.sdiameter/300, -self.diameter/300)
        d_sph = longitunal_sph_aberr(nValuesOfGlass[self.indexOfGlass], focal_len, 140/150, self.sdiameter/300, -self.diameter/300)

        parHeight = calculate_paraxial_height(self.angleAlpha, focal_len)
        tanHeight = calculate_tangential_coma(nValuesOfGlass[self.indexOfGlass], 140/150, parHeight, self.sdiameter/300, -self.diameter/300)
        sagHeight = tanHeight / 3

        self.label6 = self.createLabel(f"Focal length(cm): {focal_len:.3f}")
        self.layoutFocLen.addWidget(self.label6)

        #Spherical aberrations widgets        
        self.label7 = self.createLabel(f"Long.sph.ab.(cm): {d_sph:.3f}")
        self.label8 = self.createLabel(f"Paraxial focus")
        self.label8.setStyleSheet("font-size: 16px; font-weight: bold; color: red")
        self.label9 = self.createLabel(f"Marginal focus")
        self.label9.setStyleSheet("font-size: 16px; font-weight: bold; color: blue")

        self.layoutSph.addWidget(self.label7)
        self.layoutSph.addWidget(self.label8)
        self.layoutSph.addWidget(self.label9)

        #Coma Widgets
        self.label10 = self.createLabel(f"Par. height(cm): {parHeight:.3f}")
        self.label11 = self.createLabel(f"Sag. coma(cm): {sagHeight:.3f}")
        self.label12 = self.createLabel(f"Tan. coma(cm): {tanHeight:.3f}")
        self.label13 = self.createLabel(f"Par. img. point")
        self.label13.setStyleSheet("font-size: 16px; font-weight: bold; color: red")
        self.label14 = self.createLabel(f"Sag. img. point")
        self.label14.setStyleSheet("font-size: 16px; font-weight: bold; color: green")
        self.label15 = self.createLabel(f"Tan. img. point")
        self.label15.setStyleSheet("font-size: 16px; font-weight: bold; color: blue")
        self.label16 = self.createLabel(f"Alpha angle(degrees): 10")

        self.angleSlider = self.createHorizontalSlider(10, 60, 5, 10)
        self.angleSlider.valueChanged.connect(self.angle_slider_value_changed)

        self.layoutComa1.addWidget(self.label10)
        self.layoutComa1.addWidget(self.label11)
        self.layoutComa1.addWidget(self.label12)

        self.layoutComa2.addWidget(self.label13)
        self.layoutComa2.addWidget(self.label14)
        self.layoutComa2.addWidget(self.label15)

        self.layoutComa3.addWidget(self.label16)
        self.layoutComa3.addWidget(self.angleSlider)

        self.hide_layout(self.layoutComa1)
        self.hide_layout(self.layoutComa2)
        self.hide_layout(self.layoutComa3)

        #Chromatic widgets
        self.label17 = self.createLabel(f"1st ray wavelength(nm): 380")
        self.label18 = self.createLabel(f"2nd ray wavelength(nm): 750")

        self.ray1Slider = self.createHorizontalSlider(380, 750, 50, 380)
        self.ray1Slider.valueChanged.connect(self.ray1_color_value_changed)
        self.ray2Slider = self.createHorizontalSlider(380, 750, 50, 750)
        self.ray2Slider.valueChanged.connect(self.ray2_color_value_changed)

        self.label19 = self.createLabel(f"1st ray distance(cm): 3.098")
        self.label20 = self.createLabel(f"2nd ray distance(cm): 3.217")

        self.layoutChroma1.addWidget(self.label17)
        self.layoutChroma1.addWidget(self.ray1Slider)

        self.layoutChroma2.addWidget(self.label18)
        self.layoutChroma2.addWidget(self.ray2Slider)

        self.layoutChroma3.addWidget(self.label19)
        self.layoutChroma3.addWidget(self.label20)

        self.hide_layout(self.layoutChroma1)
        self.hide_layout(self.layoutChroma2)
        self.hide_layout(self.layoutChroma3)


        #layout set
        for i in range(14):
            for j in range(5):
                if i == 0 and j == 0:
                    self.mainLayout.addLayout(layoutLeft, i, j)
                if i == 0 and j == 1:
                    self.mainLayout.addLayout(layoutRight, i, j)
                if i == 0 and j == 2:
                    self.mainLayout.addLayout(layoutRays, i, j)
                if i == 0 and j == 3:
                    self.mainLayout.addLayout(layoutType, i, j)
                if i == 0 and j == 4:
                    self.mainLayout.addLayout(layoutAberr, i, j)
                if i == 1 and j == 0:
                    self.mainLayout.addLayout(self.layoutFocLen, i, j)
                    self.mainLayout.addLayout(self.layoutChroma3, i, j)
                if i == 2 and j == 0:
                    self.mainLayout.addLayout(self.layoutSph, i, j)
                    self.mainLayout.addLayout(self.layoutComa3, i, j)
                    self.mainLayout.addLayout(self.layoutChroma1, i, j)
                if i == 2 and j == 1:
                    self.mainLayout.addLayout(self.layoutComa1, i, j)
                    self.mainLayout.addLayout(self.layoutChroma2, i, j)
                if i == 2 and j == 2:
                    self.mainLayout.addLayout(self.layoutComa2, i, j)
                else:
                    self.mainLayout.addItem(QSpacerItem(0, 0, QSizePolicy.Expanding, QSizePolicy.Expanding), i, j)
        
        self.setLayout(self.mainLayout)

    def createLabel(self, text):
        label = QLabel(text)
        label.setAlignment(Qt.AlignCenter)
        label.setStyleSheet("font-size: 16px; font-weight: bold;")
        return label
    
    def createHorizontalSlider(self, minimum, maximum, tickInterval, value):
        slider = QSlider(Qt.Horizontal)
        slider.setMinimum(minimum)
        slider.setMaximum(maximum)
        slider.setTickInterval(tickInterval)
        slider.setTickPosition(QSlider.TicksBelow)
        slider.setValue(value)
        return slider

    def create_combobox(self, items):
        combobox = QComboBox()
        combobox.addItems(items)
        return combobox
    
    def ray1_color_value_changed(self, value):
        self.label17.setText(f"1st ray wavelength(nm): {value}")
        self.firstRayVL = value
        self.update()

        firstRayDis = self.firstRayDis/150
        self.label19.setText(f"1st ray distance(cm): {firstRayDis:.3f}")

    def ray2_color_value_changed(self, value):
        self.label18.setText(f"2nd ray wavelength(nm): {value}")
        self.secondRayVL = value
        self.update()

        secondRayDis = self.secondRayDis/150
        self.label20.setText(f"2nd ray distance(cm): {secondRayDis:.3f}")
    
    def right_slider_value_changed(self, value):
        self.label1.setText(f"RightRadius(cm): {value/1000}")
        self.diameter = value*0.3
        self.update()
        
        focal_len = self.focalLength/150
        d_sph = self.dSph/150
        self.label6.setText(f"Focal length(cm): {focal_len:.3f}")
        self.label7.setText(f"Long.sph.ab.(cm): {d_sph:.3f}")

        parHeight = self.parHeight/150
        sagHeight = self.sagHeight/150
        tanHeight = self.tanHeight/150
        self.label10.setText(f"Par. height(cm): {parHeight:.3f}")
        self.label11.setText(f"Sag. coma(cm): {sagHeight:.3f}")
        self.label12.setText(f"Tan. coma(cm): {tanHeight:.3f}")

        firstRayDis = self.firstRayDis/150
        secondRayDis = self.secondRayDis/150
        
        self.label19.setText(f"1st ray distance(cm): {firstRayDis:.3f}")
        self.label20.setText(f"2nd ray distance(cm): {secondRayDis:.3f}")
    
    def left_slider_value_changed(self, value):
        self.label2.setText(f"LeftRadius(cm): {value/1000}")
        self.sdiameter = value*0.3
        self.update()
        
        focal_len = self.focalLength/150
        d_sph = self.dSph/150
        self.label6.setText(f"Focal length(cm): {focal_len:.3f}")
        self.label7.setText(f"Long.sph.ab.(cm): {d_sph:.3f}")

        parHeight = self.parHeight/150
        sagHeight = self.sagHeight/150
        tanHeight = self.tanHeight/150
        self.label10.setText(f"Par. height(cm): {parHeight:.3f}")
        self.label11.setText(f"Sag. coma(cm): {sagHeight:.3f}")
        self.label12.setText(f"Tan. coma(cm): {tanHeight:.3f}")

        firstRayDis = self.firstRayDis/150
        secondRayDis = self.secondRayDis/150
        
        self.label19.setText(f"1st ray distance(cm): {firstRayDis:.3f}")
        self.label20.setText(f"2nd ray distance(cm): {secondRayDis:.3f}")
    
    def ray_slider_value_changed(self, value):
        self.label3.setText(f"# of Rays: {value}")
        self.nOfRays = value
        self.update()

    def type_glass_value_changed(self, value):
        self.indexOfGlass = value
        self.update()
        
        focal_len = self.calculate_focal_length(nValuesOfGlass[self.indexOfGlass], self.sdiameter/300, -self.diameter/300)
        d_sph = longitunal_sph_aberr(nValuesOfGlass[self.indexOfGlass], focal_len, 140/150, self.sdiameter/300, -self.diameter/300)

        self.label6.setText(f"Focal length(cm): {focal_len:.3f}")
        self.label7.setText(f"Long.sph.ab.(cm): {d_sph:.3f}")

        parHeight = calculate_paraxial_height(self.angleAlpha, focal_len)
        tanHeight = calculate_tangential_coma(nValuesOfGlass[self.indexOfGlass], 140/150, parHeight, self.sdiameter/300, -self.diameter/300)
        sagHeight = tanHeight / 3
        self.label10.setText(f"Par. height(cm): {parHeight:.3f}")
        self.label11.setText(f"Sag. coma(cm): {sagHeight:.3f}")
        self.label12.setText(f"Tan. coma(cm): {tanHeight:.3f}")

        firstRayDis = self.calculate_focal_length(calculate_refractive_index(self.firstRayVL, nValuesOfGlass[self.indexOfGlass], HValuesOfGlass[self.indexOfGlass]), self.sdiameter/300, -self.diameter/300)
        secondRayDis = self.calculate_focal_length(calculate_refractive_index(self.secondRayVL, nValuesOfGlass[self.indexOfGlass], HValuesOfGlass[self.indexOfGlass]), self.sdiameter/300, -self.diameter/300)

        self.label19.setText(f"1st ray distance(cm): {firstRayDis:.3f}")
        self.label20.setText(f"2nd ray distance(cm): {secondRayDis:.3f}")

    def angle_slider_value_changed(self, value):
        self.label16.setText(f"Alpha angle(degrees): {value}")
        self.angleAlpha = value
        self.update()

        parHeight = self.parHeight/150
        sagHeight = self.sagHeight/150
        tanHeight = self.tanHeight/150
        self.label10.setText(f"Par. height(cm): {parHeight:.3f}")
        self.label11.setText(f"Sag. coma(cm): {sagHeight:.3f}")
        self.label12.setText(f"Tan. coma(cm): {tanHeight:.3f}")

    def aberr_value_changed(self, value):
        self.aberr = value
        self.update()
        if(value == 0):
            self.hide_layout(self.layoutChroma1)
            self.hide_layout(self.layoutChroma2)
            self.hide_layout(self.layoutChroma3)
            self.hide_layout(self.layoutComa1)
            self.hide_layout(self.layoutComa2)
            self.hide_layout(self.layoutComa3)
            self.show_layout(self.layoutFocLen)
            self.show_layout(self.layoutSph)
        elif(value == 1):
            self.hide_layout(self.layoutChroma1)
            self.hide_layout(self.layoutChroma2)
            self.hide_layout(self.layoutChroma3)
            self.show_layout(self.layoutComa1)
            self.show_layout(self.layoutComa2)
            self.show_layout(self.layoutComa3)
            self.show_layout(self.layoutFocLen)
            self.hide_layout(self.layoutSph)
        elif(value == 2):
            self.show_layout(self.layoutChroma1)
            self.show_layout(self.layoutChroma2)
            self.show_layout(self.layoutChroma3)
            self.hide_layout(self.layoutComa1)
            self.hide_layout(self.layoutComa2)
            self.hide_layout(self.layoutComa3)
            self.hide_layout(self.layoutFocLen)
            self.hide_layout(self.layoutSph)

    def show_layout(self, layout):
        for i in range(layout.count()):
            item = layout.itemAt(i)
            if item.widget():
                item.widget().setVisible(True)
            elif item.layout():
                self.show_layout(item.layout())

    def hide_layout(self, layout):
        for i in range(layout.count()):
            item = layout.itemAt(i)
            if item.widget():
                item.widget().setVisible(False)
            elif item.layout():
                self.hide_layout(item.layout())

    def calculate_focal_length(self, n, r1, r2):
        return ((n-1) * ((1/r1) - (1/r2)))**(-1)

    def paintEvent(self, event):
        height = self.height()
        width = self.width()
        radius = self.diameter/2
        angle = degrees(asin(150/radius))
        currentGlass = nValuesOfGlass[self.indexOfGlass]
        currentHOfGlass = HValuesOfGlass[self.indexOfGlass]

        sradius = self.sdiameter/2
        sangle = degrees(asin(150/sradius))
        
        #spherical aberration
        self.focalLength = self.calculate_focal_length(currentGlass, sradius, -radius)
        self.dSph = longitunal_sph_aberr(currentGlass, self.focalLength, 140, sradius, -radius)

        #coma
        self.parHeight = calculate_paraxial_height(self.angleAlpha, self.focalLength)
        self.tanHeight = calculate_tangential_coma(currentGlass, 140, self.parHeight, sradius, -radius)
        self.sagHeight = self.tanHeight / 3

        #chromatic aberration
        self.firstRayN = calculate_refractive_index(self.firstRayVL, currentGlass, currentHOfGlass)
        self.secondRayN = calculate_refractive_index(self.secondRayVL, currentGlass, currentHOfGlass)
        self.firstRayDis = self.calculate_focal_length(self.firstRayN, sradius, -radius)
        self.secondRayDis = self.calculate_focal_length(self.secondRayN, sradius, -radius)

        offset = int(radius - sqrt(radius**2 - 150**2))
        soffset = int(sradius - sqrt(sradius**2 - 150**2))
        
        painter = QPainter(self)
        painter.setPen(QPen(Qt.white, 3, Qt.SolidLine))
        #painter.drawRect(int((width/2)-(300/2)), int((height/2) - (300/2)), 300, 300)
        #painter.drawLine(int((width/2)), int((height/2) - (300/2)), int((width/2)), int((height/2) + (300/2)))
        #painter.drawLine(int((width/2)-(300/2)), int((height/2)), int((width/2)+(300/2)), int((height/2)))
        
        painter.drawArc(int((width/2)-(self.diameter-offset)), int((height/2) - (self.diameter/2)), int(self.diameter), int(self.diameter), int((-angle)*16), int(2*angle*16))

        painter.drawArc(int((width/2)-soffset), int((height/2) - (self.sdiameter/2)), int(self.sdiameter), int(self.sdiameter), int((180-sangle)*16), int(2*sangle*16))

        if(self.aberr == 0):
            #Paint Rays
            painter.setPen(QPen(Qt.yellow, 1, Qt.SolidLine))
            
            positionsOfRays = np.linspace(int((height/2)-140), int((height/2)+140), self.nOfRays)
            
            for ray in positionsOfRays:
                first_x_pos_end = min(find_intersection_x(0 , ray, int(width/2) + (sradius - soffset), int(height/2), sradius))
                painter.drawLine(0, int(ray), int(first_x_pos_end), int(ray))
                
                ray_vector = np.array([1,0])
                normal_vector = normal_on_circle(first_x_pos_end, ray, int(width/2) + (sradius - soffset), int(height/2))
                inAngle = angle_between_vectors(ray_vector, normal_vector)
                outAngle = angle_on_exit(1.0, currentGlass, inAngle)
                outSlope = np.tan(outAngle)
                
                if(ray >= int(height/2)):
                    second_x_pos_end, second_y_pos_end = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, -outSlope)
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end), int(second_y_pos_end))
                    deltaPos = second_x_pos_end - first_x_pos_end

                    distancefromParaxial = ((ray - height/2)/140) * self.dSph
                    finalSlope = getSlopeFromPoints(int(second_x_pos_end), int(ray - (deltaPos * outSlope)), int((width/2 + self.focalLength) - distancefromParaxial), int(height/2))
                    painter.drawLine(int(second_x_pos_end), int(ray - (deltaPos * outSlope)), width, int((ray - (deltaPos * outSlope)) + (width - second_x_pos_end) * finalSlope))
                else:
                    second_x_pos_end, second_y_pos_end = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, outSlope)
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end), int(second_y_pos_end))
                    deltaPos = second_x_pos_end - first_x_pos_end

                    distancefromParaxial = -((ray - height/2)/140) * self.dSph
                    finalSlope = getSlopeFromPoints(int(second_x_pos_end), int(ray + (deltaPos * outSlope)), int((width/2 + self.focalLength) - distancefromParaxial), int(height/2))
                    painter.drawLine(int(second_x_pos_end), int(ray + (deltaPos * outSlope)), width, int((ray + (deltaPos * outSlope)) + (width - second_x_pos_end) * finalSlope))

            #Paint paraxial focal point
            painter.setPen(QPen(Qt.red, 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.focalLength), int(height/2))

            #Paint longitudinal sph aberr point
            painter.setPen(QPen(Qt.blue, 10, Qt.SolidLine))
            painter.drawPoint(int((width/2 + self.focalLength) - self.dSph), int(height/2))
        elif(self.aberr == 1):
            painter.setPen(QPen(Qt.yellow, 1, Qt.SolidLine))
            positionsOfRays = np.linspace(int((height/2)-140), int((height/2)+140), self.nOfRays)
            slopeAlpha = np.tan(np.radians(-self.angleAlpha))
            for ray in positionsOfRays:
                first_x_pos_end = min(find_intersection_x(0 , ray, int(width/2) + (sradius - soffset), int(height/2), sradius))
                constanOfRay = ray - slopeAlpha * first_x_pos_end
                beginX = (height - constanOfRay) / slopeAlpha
                painter.drawLine(int(beginX), int(height), int(first_x_pos_end), int(ray))

                ray_vector = np.array([1,-slopeAlpha])
                normal_vector = normal_on_circle(first_x_pos_end, ray, int(width/2) + (sradius - soffset), int(height/2))
                inAngle = angle_between_vectors(ray_vector, normal_vector)
                outAngle = angle_on_exit(1.0, currentGlass, inAngle)
                outSlope = np.tan(outAngle)
                second_x_pos_end, second_y_pos_end = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, -outSlope)
                painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end), int(second_y_pos_end))

                deltaComa = self.tanHeight - self.sagHeight
                rayHeight = abs(ray - (height/2))/140
                painter.drawLine(int(second_x_pos_end), int(second_y_pos_end), int(width/2 + self.focalLength), int(height/2 - self.parHeight - self.sagHeight - (rayHeight * deltaComa)))

                if(ray == int(height/2)):
                    painter.setPen(QPen(Qt.yellow, 1, Qt.DashLine))
                    painter.drawLine(int(second_x_pos_end), int(second_y_pos_end), int(width/2 + self.focalLength), int(height/2 - self.parHeight))
                    painter.setPen(QPen(Qt.yellow, 1, Qt.SolidLine))

            #Paint axis
            painter.setPen(QPen(Qt.white, 1, Qt.DashLine))
            painter.drawLine(0, int(height/2), width, int(height/2))

            #Paint paraxial focal point
            painter.setPen(QPen(Qt.red, 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.focalLength), int((height/2) - self.parHeight))

            #Paint tangential coma point
            painter.setPen(QPen(Qt.blue, 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.focalLength), int((height/2) - self.parHeight - self.tanHeight))
            
            #Paint sagittal coma point
            painter.setPen(QPen(Qt.green, 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.focalLength), int((height/2) - self.parHeight - self.sagHeight))

        elif(self.aberr == 2):
            #Paint Rays
            painter.setPen(QPen(Qt.white, 1, Qt.SolidLine))

            red1, green1, blue1 = wavelen_to_rgb(self.firstRayVL)
            red2, green2, blue2 = wavelen_to_rgb(self.secondRayVL)
            
            positionsOfRays = np.linspace(int((height/2)-140), int((height/2)+140), self.nOfRays)
            
            for ray in positionsOfRays:
                first_x_pos_end = min(find_intersection_x(0 , ray, int(width/2) + (sradius - soffset), int(height/2), sradius))
                painter.drawLine(0, int(ray), int(first_x_pos_end), int(ray))
                
                ray_vector = np.array([1,0])
                normal_vector = normal_on_circle(first_x_pos_end, ray, int(width/2) + (sradius - soffset), int(height/2))
                inAngle = angle_between_vectors(ray_vector, normal_vector)
                outAngle1 = angle_on_exit(1.0, self.firstRayN, inAngle)
                outSlope1 = np.tan(outAngle1)

                outAngle2 = angle_on_exit(1.0, self.secondRayN, inAngle)
                outSlope2 = np.tan(outAngle2)
                
                if(ray >= int(height/2)):
                    second_x_pos_end1, second_y_pos_end1 = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, -outSlope1)
                    second_x_pos_end2, second_y_pos_end2 = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, -outSlope2)
                    
                    painter.setPen(QPen(QColor(red1, green1, blue1), 1, Qt.SolidLine))
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end1), int(second_y_pos_end1))
                    painter.setPen(QPen(QColor(red2, green2, blue2), 1, Qt.SolidLine))
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end2), int(second_y_pos_end2))
                    
                    deltaPos1 = second_x_pos_end1 - first_x_pos_end
                    deltaPos2 = second_x_pos_end2 - first_x_pos_end

                    finalSlope1 = getSlopeFromPoints(int(second_x_pos_end1), int(ray - (deltaPos1 * outSlope1)), int((width/2 + self.firstRayDis)), int(height/2))
                    finalSlope2 = getSlopeFromPoints(int(second_x_pos_end2), int(ray - (deltaPos2 * outSlope2)), int((width/2 + self.secondRayDis)), int(height/2))

                    painter.setPen(QPen(QColor(red1, green1, blue1), 1, Qt.SolidLine))
                    painter.drawLine(int(second_x_pos_end1), int(ray - (deltaPos1 * outSlope1)), width, int((ray - (deltaPos1 * outSlope1)) + (width - second_x_pos_end1) * finalSlope1))
                    painter.setPen(QPen(QColor(red2, green2, blue2), 1, Qt.SolidLine))
                    painter.drawLine(int(second_x_pos_end2), int(ray - (deltaPos2 * outSlope2)), width, int((ray - (deltaPos2 * outSlope2)) + (width - second_x_pos_end2) * finalSlope2))
                    painter.setPen(QPen(Qt.white, 1, Qt.SolidLine))
                else:
                    second_x_pos_end1, second_y_pos_end1 = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, outSlope1)
                    second_x_pos_end2, second_y_pos_end2 = point_on_circle(first_x_pos_end, ray, int(width/2) - (radius - offset), int(height/2), radius, outSlope2)
                    
                    painter.setPen(QPen(QColor(red1, green1, blue1), 1, Qt.SolidLine))
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end1), int(second_y_pos_end1))
                    painter.setPen(QPen(QColor(red2, green2, blue2), 1, Qt.SolidLine))
                    painter.drawLine(int(first_x_pos_end), int(ray), int(second_x_pos_end2), int(second_y_pos_end2))
                    
                    deltaPos1 = second_x_pos_end1 - first_x_pos_end
                    deltaPos2 = second_x_pos_end2 - first_x_pos_end

                    finalSlope1 = getSlopeFromPoints(int(second_x_pos_end1), int(ray + (deltaPos1 * outSlope1)), int((width/2 + self.firstRayDis)), int(height/2))
                    finalSlope2 = getSlopeFromPoints(int(second_x_pos_end2), int(ray + (deltaPos2 * outSlope2)), int((width/2 + self.secondRayDis)), int(height/2))

                    painter.setPen(QPen(QColor(red1, green1, blue1), 1, Qt.SolidLine))
                    painter.drawLine(int(second_x_pos_end1), int(ray + (deltaPos1 * outSlope1)), width, int((ray + (deltaPos1 * outSlope1)) + (width - second_x_pos_end1) * finalSlope1))
                    painter.setPen(QPen(QColor(red2, green2, blue2), 1, Qt.SolidLine))
                    painter.drawLine(int(second_x_pos_end2), int(ray + (deltaPos2 * outSlope2)), width, int((ray + (deltaPos2 * outSlope2)) + (width - second_x_pos_end2) * finalSlope2))
                    painter.setPen(QPen(Qt.white, 1, Qt.SolidLine))

            #Paint first ray focal point
            red, green, blue = wavelen_to_rgb(self.firstRayVL)
            painter.setPen(QPen(QColor(red, green, blue), 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.firstRayDis), int(height/2))

            #Paint longitudinal sph aberr point
            red, green, blue = wavelen_to_rgb(self.secondRayVL)
            painter.setPen(QPen(QColor(red, green, blue), 10, Qt.SolidLine))
            painter.drawPoint(int(width/2 + self.secondRayDis), int(height/2))

def calculate_refractive_index(wavelength, n, H):
    return n + (H/wavelength**2)

def wavelen_to_rgb(wavelength):
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

    if wavelength < 420:
        factor = 0.3 + 0.7 * (wavelength - 380) / (420 - 380)
    elif wavelength < 645:
        factor = 1.0
    else:
        factor = 0.3 + 0.7 * (750 - wavelength) / (750 - 645)

    red *= factor
    green *= factor
    blue *= factor

    return int(red*255), int(green*255), int(blue*255)

def point_on_circle(x, y, cx, cy, r, slope):
    distance = (x-cx)**2 + (y-cy)**2 - r**2
    if distance < 0:
        return point_on_circle(x+1, y + slope, cx, cy, r, slope)
    else:
        return x, y

def calculate_tangential_coma(n, y, h, r1, r2):
    g5 = 2 * (n**2 - 1)
    g8 = n * (n - 1) / 2

    c1 = 1 / r1
    c2 = 1 / r2
    c = c1 - c2

    g = (g5 * c * c1)/4 - (g8 * (c**2))

    return (-3 * g * y**2 * h)

def calculate_paraxial_height(alpha, f):
    return np.tan(np.radians(alpha)) * f

def getSlopeFromPoints(x0,y0,x1,y1):
    x = x1 - x0
    y = y1 - y0
    return y/x            

def angle_on_exit(n1, n2, theta):
    first = (n1/n2) * np.sin(theta)
    return np.arcsin(first)

def normal_on_circle(x, y, center_x, center_y):
    return np.array([center_x - x, center_y - y])

def angle_between_vectors(v1, v2):
    dotValue = np.dot(v1, v2)
    magnitudeV1 = np.linalg.norm(v1)
    magnitudeV2 = np.linalg.norm(v2)
    return np.arccos(dotValue/(magnitudeV1 * magnitudeV2))

def calculate_y_coordinate(x, center_x, center_y, radius):
    # Calculate the squared distance between the x-coordinate and the center of the circle
    x_distance_squared = (x - center_x) ** 2
    
    # Calculate the squared distance between the y-coordinate and the center of the circle
    y_distance_squared = radius**2 - x_distance_squared
    
    # If the squared distance is negative, the point is outside the circle
    if y_distance_squared < 0:
        return None
    
    # Calculate the y-coordinate using the positive square root
    y = center_y + sqrt(y_distance_squared)
    
    return y

def longitunal_sph_aberr(n, f, y, r1, r2):
    g1 = (n**2)*(n - 1) / 2
    g2 = (2*n + 1) * (n - 1) / 2
    g4 = (n + 2) * (n - 1) / (2*n)

    c1 = 1 / r1
    c2 = 1 / r2
    c = c1 - c2

    g_sph = g1*(c**3) - g2*(c**2)*c1 + g4*c*(c1**2)
    return (g_sph * (f**2) * (y**2))


def find_intersection_x(m, c, h, k, r):
    # Coefficients of the quadratic equation
    a = 1 + m**2
    b = -2 * (h + m * (c - k))
    c = h**2 + (c - k)**2 - r**2

    # Calculate discriminant
    discriminant = b**2 - 4*a*c

    # Check if the discriminant is non-negative (real roots)
    if discriminant >= 0:
        # Calculate the x-coordinates of the intersection points
        x1 = (-b + sqrt(discriminant)) / (2*a)
        x2 = (-b - sqrt(discriminant)) / (2*a)
        return x1, x2
    else:
        return 0,0  # No real roots (no intersection)

class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()

        self.setGeometry(200, 200, 1280, 720)
        self.setWindowTitle("FYO projekt: Aberace cocek")
        self.setStyleSheet("background-color: black; color: white")

        self.centralWidget = CentralWidget()
        self.setCentralWidget(self.centralWidget)



if __name__ == "__main__":
    app = QApplication(sys.argv)
    window = MainWindow()
    window.show()
    sys.exit(app.exec())